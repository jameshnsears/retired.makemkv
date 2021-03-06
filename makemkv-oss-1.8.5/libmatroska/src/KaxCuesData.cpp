/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
**
** <file/class description>
**
** Copyright (C) 2002-2010 Steve Lhomme.  All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
** See http://www.matroska.org/license/lgpl/ for LGPL licensing information.**
** Contact license@matroska.org if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

/*!
	\file
	\version \$Id: KaxCuesData.cpp 1265 2007-01-14 17:20:35Z mosu $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include <lgpl/cassert>

#include "matroska/KaxCuesData.h"
#include "matroska/KaxContexts.h"
#include "matroska/KaxBlock.h"
#include "matroska/KaxBlockData.h"
#include "matroska/KaxCluster.h"
#include "matroska/KaxSegment.h"
#include "matroska/KaxDefines.h"

START_LIBMATROSKA_NAMESPACE

DEFINE_START_SEMANTIC(KaxCuePoint)
DEFINE_SEMANTIC_ITEM(true, true, KaxCueTime)
DEFINE_SEMANTIC_ITEM(true, false, KaxCueTrackPositions)
DEFINE_END_SEMANTIC(KaxCuePoint)

DEFINE_START_SEMANTIC(KaxCueTrackPositions)
DEFINE_SEMANTIC_ITEM(true, true, KaxCueTrack)
DEFINE_SEMANTIC_ITEM(true, true, KaxCueClusterPosition)
DEFINE_SEMANTIC_ITEM(false, true, KaxCueBlockNumber)
#if MATROSKA_VERSION >= 2
DEFINE_SEMANTIC_ITEM(false, true, KaxCueCodecState)
DEFINE_SEMANTIC_ITEM(false, false, KaxCueReference)
#endif // MATROSKA_VERSION
DEFINE_END_SEMANTIC(KaxCueTrackPositions)

#if MATROSKA_VERSION >= 2
DEFINE_START_SEMANTIC(KaxCueReference)
DEFINE_SEMANTIC_ITEM(true, true, KaxCueRefTime)
DEFINE_SEMANTIC_ITEM(true, true, KaxCueRefCluster)
DEFINE_SEMANTIC_ITEM(false, true, KaxCueRefNumber)
DEFINE_SEMANTIC_ITEM(false, true, KaxCueRefCodecState)
DEFINE_END_SEMANTIC(KaxCueReference)
#endif // MATROSKA_VERSION

DEFINE_MKX_MASTER      (KaxCuePoint,           0xBB, 1, KaxCues, "CuePoint");
DEFINE_MKX_UINTEGER    (KaxCueTime,            0xB3, 1, KaxCuePoint, "CueTime");
DEFINE_MKX_MASTER      (KaxCueTrackPositions,  0xB7, 1, KaxCuePoint, "CueTrackPositions");
DEFINE_MKX_UINTEGER    (KaxCueTrack,           0xF7, 1, KaxCueTrackPositions, "CueTrack");
DEFINE_MKX_UINTEGER    (KaxCueClusterPosition, 0xF1, 1, KaxCueTrackPositions, "CueClusterPosition");
DEFINE_MKX_UINTEGER_DEF(KaxCueBlockNumber,   0x5378, 2, KaxCueTrackPositions, "CueBlockNumber", 1);
#if MATROSKA_VERSION >= 2
DEFINE_MKX_UINTEGER_DEF(KaxCueCodecState,      0xEA, 1, KaxCueTrackPositions, "CueCodecState", 0);
DEFINE_MKX_MASTER      (KaxCueReference,       0xDB, 1, KaxCueTrackPositions, "CueReference");
DEFINE_MKX_UINTEGER    (KaxCueRefTime,         0x96, 1, KaxCueReference, "CueRefTime");
DEFINE_MKX_UINTEGER    (KaxCueRefCluster,      0x97, 1, KaxCueRefTime, "CueRefCluster");
DEFINE_MKX_UINTEGER_DEF(KaxCueRefNumber,     0x535F, 2, KaxCueRefTime, "CueRefNumber", 1);
DEFINE_MKX_UINTEGER_DEF(KaxCueRefCodecState,   0xEB, 1, KaxCueRefTime, "CueRefCodecState", 0);
#endif

/*!
	\todo handle codec state checking
	\todo remove duplicate references (reference to 2 frames that each reference the same frame)
*/
void KaxCuePoint::PositionSet(const KaxBlockGroup & BlockReference, uint64 GlobalTimecodeScale)
{
	// fill me
	KaxCueTime & NewTime = GetChild<KaxCueTime>(*this);
	*static_cast<EbmlUInteger*>(&NewTime) = BlockReference.GlobalTimecode() / GlobalTimecodeScale;

	KaxCueTrackPositions & NewPositions = AddNewChild<KaxCueTrackPositions>(*this);
	KaxCueTrack & TheTrack = GetChild<KaxCueTrack>(NewPositions);
	*static_cast<EbmlUInteger*>(&TheTrack) = BlockReference.TrackNumber();
	
	KaxCueClusterPosition & TheClustPos = GetChild<KaxCueClusterPosition>(NewPositions);
	*static_cast<EbmlUInteger*>(&TheClustPos) = BlockReference.ClusterPosition();

#if MATROSKA_VERSION >= 2
	// handle reference use
	if (BlockReference.ReferenceCount() != 0)
	{
		unsigned int i;
		for (i=0; i<BlockReference.ReferenceCount(); i++) {
			KaxCueReference & NewRefs = AddNewChild<KaxCueReference>(NewPositions);
			NewRefs.AddReference(BlockReference.Reference(i).RefBlock(), GlobalTimecodeScale);
		}
	}

	KaxCodecState *CodecState = static_cast<KaxCodecState *>(BlockReference.FindFirstElt(EBML_INFO(KaxCodecState)));
	if (CodecState != NULL) {
		KaxCueCodecState &CueCodecState = AddNewChild<KaxCueCodecState>(NewPositions);
		*static_cast<EbmlUInteger*>(&CueCodecState) = BlockReference.GetParentCluster()->GetParentSegment()->GetRelativePosition(CodecState->GetElementPosition());
	}
#endif // MATROSKA_VERSION

	SetValueIsSet();
}

void KaxCuePoint::PositionSet(const KaxBlockBlob & BlobReference, uint64 GlobalTimecodeScale)
{
	const KaxInternalBlock &BlockReference = BlobReference;
	const KaxBlockGroup* BlockGroupPointer = NULL;

	if (!BlobReference.IsSimpleBlock()) {
		const KaxBlockGroup &BlockGroup = BlobReference;
		BlockGroupPointer = & BlockGroup;
	}
	PositionSet(BlockReference,BlockGroupPointer,GlobalTimecodeScale);
}

void KaxCuePoint::PositionSet(const KaxSimpleBlock & BlockReference, uint64 GlobalTimecodeScale)
{
	PositionSet(BlockReference,NULL,GlobalTimecodeScale);
}

void KaxCuePoint::PositionSet(const KaxInternalBlock & BlockReference, const KaxBlockGroup* BlockGroup, uint64 GlobalTimecodeScale)
{
	// fill me
	KaxCueTime & NewTime = GetChild<KaxCueTime>(*this);
	*static_cast<EbmlUInteger*>(&NewTime) = BlockReference.GlobalTimecode() / GlobalTimecodeScale;

	KaxCueTrackPositions & NewPositions = AddNewChild<KaxCueTrackPositions>(*this);
	KaxCueTrack & TheTrack = GetChild<KaxCueTrack>(NewPositions);
	*static_cast<EbmlUInteger*>(&TheTrack) = BlockReference.TrackNum();
	
	KaxCueClusterPosition & TheClustPos = GetChild<KaxCueClusterPosition>(NewPositions);
	*static_cast<EbmlUInteger*>(&TheClustPos) = BlockReference.ClusterPosition();

#if 0 // MATROSKA_VERSION >= 2
	// handle reference use
	if (BlockReference.ReferenceCount() != 0)
	{
		unsigned int i;
		for (i=0; i<BlockReference.ReferenceCount(); i++) {
			KaxCueReference & NewRefs = AddNewChild<KaxCueReference>(NewPositions);
			NewRefs.AddReference(BlockReference.Reference(i).RefBlock(), GlobalTimecodeScale);
		}
	}
#endif // MATROSKA_VERSION

#if MATROSKA_VERSION >= 2
	if (BlockGroup!=NULL) {
		const KaxCodecState *CodecState = static_cast<KaxCodecState *>(BlockGroup->FindFirstElt(EBML_INFO(KaxCodecState)));
		if (CodecState != NULL) {
			KaxCueCodecState &CueCodecState = AddNewChild<KaxCueCodecState>(NewPositions);
			*static_cast<EbmlUInteger*>(&CueCodecState) = BlockGroup->GetParentCluster()->GetParentSegment()->GetRelativePosition(CodecState->GetElementPosition());
		}
	}
#endif // MATROSKA_VERSION

	SetValueIsSet();
}

#if MATROSKA_VERSION >= 2
/*!
	\todo handle codec state checking
*/
void KaxCueReference::AddReference(const KaxBlockBlob & BlockReference, uint64 GlobalTimecodeScale)
{
	const KaxInternalBlock & theBlock = BlockReference;
	KaxCueRefTime & NewTime = GetChild<KaxCueRefTime>(*this);
	*static_cast<EbmlUInteger*>(&NewTime) = theBlock.GlobalTimecode() / GlobalTimecodeScale;

	KaxCueRefCluster & TheClustPos = GetChild<KaxCueRefCluster>(*this);
	*static_cast<EbmlUInteger*>(&TheClustPos) = theBlock.ClusterPosition();

#ifdef OLD
	// handle recursive reference use
	if (BlockReference.ReferenceCount() != 0)
	{
		unsigned int i;
		for (i=0; i<BlockReference.ReferenceCount(); i++) {
			AddReference(BlockReference.Reference(i).RefBlock());
		}
	}
#endif /* OLD */
}
#endif

bool KaxCuePoint::IsSmallerThan(const EbmlElement * EltB) const
{
	assert(EbmlId(*this) == EBML_ID(KaxCuePoint));
	assert(EbmlId(*EltB) == EBML_ID(KaxCuePoint));

	const KaxCuePoint & theEltB = *static_cast<const KaxCuePoint *>(EltB);

	// compare timecode
	const KaxCueTime * TimeCodeA = static_cast<const KaxCueTime *>(FindElt(EBML_INFO(KaxCueTime)));
	if (TimeCodeA == NULL)
		return false;

	const KaxCueTime * TimeCodeB = static_cast<const KaxCueTime *>(theEltB.FindElt(EBML_INFO(KaxCueTime)));
	if (TimeCodeB == NULL)
		return false;

	if (TimeCodeA->IsSmallerThan(TimeCodeB))
		return true;

	if (TimeCodeB->IsSmallerThan(TimeCodeA))
		return false;

	// compare tracks (timecodes are equal)
	const KaxCueTrack * TrackA = static_cast<const KaxCueTrack *>(FindElt(EBML_INFO(KaxCueTrack)));
	if (TrackA == NULL)
		return false;

	const KaxCueTrack * TrackB = static_cast<const KaxCueTrack *>(theEltB.FindElt(EBML_INFO(KaxCueTrack)));
	if (TrackB == NULL)
		return false;

	if (TrackA->IsSmallerThan(TrackB))
		return true;

	if (TrackB->IsSmallerThan(TrackA))
		return false;

	return false;
}

bool KaxCuePoint::Timecode(uint64 & aTimecode, uint64 GlobalTimecodeScale) const
{
	const KaxCueTime *aTime = static_cast<const KaxCueTime *>(FindFirstElt(EBML_INFO(KaxCueTime)));
	if (aTime == NULL)
		return false;
	aTimecode = uint64(*aTime) * GlobalTimecodeScale;
	return true;
}

/*!
	\brief return the position of the Cluster to load
*/
const KaxCueTrackPositions * KaxCuePoint::GetSeekPosition() const
{
	const KaxCueTrackPositions * result = NULL;
	uint64 aPosition = EBML_PRETTYLONGINT(0xFFFFFFFFFFFFFFF);
	// find the position of the "earlier" Cluster
	const KaxCueTrackPositions *aPoss = static_cast<const KaxCueTrackPositions *>(FindFirstElt(EBML_INFO(KaxCueTrackPositions)));
	while (aPoss != NULL)
	{
		const KaxCueClusterPosition *aPos = static_cast<const KaxCueClusterPosition *>(aPoss->FindFirstElt(EBML_INFO(KaxCueClusterPosition)));
		if (aPos != NULL && uint64(*aPos) < aPosition) {
			aPosition = uint64(*aPos);
			result = aPoss;
		}
		
		aPoss = static_cast<const KaxCueTrackPositions *>(FindNextElt(*aPoss));
	}
	return result;
}

uint64 KaxCueTrackPositions::ClusterPosition() const
{
	const KaxCueClusterPosition *aPos = static_cast<const KaxCueClusterPosition *>(FindFirstElt(EBML_INFO(KaxCueClusterPosition)));
	if (aPos == NULL)
		return 0;

	return uint64(*aPos);
}

uint16 KaxCueTrackPositions::TrackNumber() const
{
	const KaxCueTrack *aTrack = static_cast<const KaxCueTrack *>(FindFirstElt(EBML_INFO(KaxCueTrack)));
	if (aTrack == NULL)
		return 0;

	return uint16(*aTrack);
}


END_LIBMATROSKA_NAMESPACE
