/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2014 Coherent Labs AD and/or its licensors. All
rights reserved in all media.

The coded instructions, statements, computer programs, and/or related
material (collectively the "Data") in these files contain confidential
and unpublished information proprietary Coherent Labs and/or its
licensors, which is protected by United States of America federal
copyright law and by international treaties.

This software or source code is supplied under the terms of a license
agreement and nondisclosure agreement with Coherent Labs Limited and may
not be copied, disclosed, or exploited except in accordance with the
terms of that agreement. The Data may not be disclosed or distributed to
third parties, in whole or in part, without the prior written consent of
Coherent Labs Limited.

COHERENT LABS MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS
SOURCE CODE FOR ANY PURPOSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER, ITS AFFILIATES,
PARENT COMPANIES, LICENSORS, SUPPLIERS, OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OR PERFORMANCE OF THIS SOFTWARE OR SOURCE CODE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "CoherentUIPluginPrivatePCH.h"

#include "CoherentUIMeshData.h"

#include "Model.h"
#include "StaticMeshResources.h"

namespace
{

void LoadMeshLODIndices(const FStaticMeshLODResources& LODResources, TArray<uint32>& Indices)
{
	auto& IndexBuffer = LODResources.IndexBuffer;
	const auto Is32Bit = IndexBuffer.Is32Bit();
	const auto NumIndices = LODResources.GetNumTriangles() * 3;
	auto IndexBufferData = RHILockIndexBuffer(IndexBuffer.IndexBufferRHI, 0, Is32Bit ? NumIndices * 4 : NumIndices * 2, RLM_ReadOnly);
	Indices.AddUninitialized(NumIndices);
	if (Is32Bit)
	{
		FMemory::Memcpy(Indices.GetData(), IndexBufferData, Indices.GetAllocatedSize());
	}
	else
	{
		auto IndexBufferUInt16 = static_cast<const uint16*>(IndexBufferData);
		for (auto i = 0; i != NumIndices; ++i)
		{
			Indices[i] = IndexBufferUInt16[i];
		}
	}
	RHIUnlockIndexBuffer(IndexBuffer.IndexBufferRHI);
}

void LoadMeshLODUVs(const FStaticMeshLODResources& LODResources, TArray<FCoherentUIVertexUV>& UVs)
{
	auto& VertexBuffer = LODResources.VertexBuffer;
	const auto NumVertices = LODResources.GetNumVertices();
	const bool UseFullUVs = VertexBuffer.GetUseFullPrecisionUVs();
	const auto Stride = VertexBuffer.GetStride();
	auto VertexBufferData = static_cast<uint8*>(RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, NumVertices * Stride, RLM_ReadOnly));

	UVs.AddUninitialized(NumVertices);
	const auto NumTexCoords = VertexBuffer.GetNumTexCoords();

	for (auto Index = 0u; Index != NumVertices; ++Index)
	{
		auto Vertex = VertexBufferData + Index * Stride;
		for (auto Channel = 0; Channel != NumTexCoords; ++Channel)
		{
			if (!UseFullUVs)
			{
				UVs[Index].Channels[Channel] = ((TStaticMeshFullVertexFloat16UVs<MAX_STATIC_TEXCOORDS>*)Vertex)->UVs[Channel];
			}
			else
			{
				UVs[Index].Channels[Channel] = ((TStaticMeshFullVertexFloat32UVs<MAX_STATIC_TEXCOORDS>*)Vertex)->UVs[Channel];
			}
		}
	}
	RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
}

void LoadMeshLODPositions(const FStaticMeshLODResources& LODResources, TArray<FVector>& Positions)
{
	auto& PositionVertex = LODResources.PositionVertexBuffer;
	const auto NumVertices = LODResources.GetNumVertices();
	const auto Stride = PositionVertex.GetStride();
	auto PositionBufferData = static_cast<const uint8*>(RHILockVertexBuffer(PositionVertex.VertexBufferRHI, 0, NumVertices * Stride, RLM_ReadOnly));
	Positions.AddUninitialized(NumVertices);
	for (auto Index = 0u; Index != NumVertices; ++Index)
	{
		auto Vertex = (FPositionVertex*)(PositionBufferData + Index * Stride);
		Positions[Index] = Vertex->Position;
	}
	RHIUnlockVertexBuffer(PositionVertex.VertexBufferRHI);
}

void LoadMeshLOD(const FStaticMeshLODResources& LODResources, FCoherentUIMeshLOD& LODData)
{
	LoadMeshLODIndices(LODResources, LODData.Indices);
	LoadMeshLODUVs(LODResources, LODData.UVs);
	LoadMeshLODPositions(LODResources, LODData.Positions);
}

}

FCoherentUIMeshData LoadMeshData(const UStaticMeshComponent* Mesh)
{
	auto& LODs = Mesh->StaticMesh->RenderData->LODResources;
	auto NumLODs = LODs.Num();
	FCoherentUIMeshData Data;
	Data.Mesh = Mesh;
	Data.LODs.Reserve(NumLODs);
	for (auto LOD = 0u; LOD != NumLODs; ++LOD)
	{
		FCoherentUIMeshLOD MeshLOD;
		LoadMeshLOD(LODs[LOD], MeshLOD);
		Data.LODs.Add(MeshLOD);
	}
	return Data;
}
