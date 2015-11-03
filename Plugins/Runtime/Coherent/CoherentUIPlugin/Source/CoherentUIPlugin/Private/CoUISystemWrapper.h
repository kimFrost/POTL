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
#pragma once

namespace Coherent
{
namespace UI
{
	struct ViewInfo;
	class ViewContext;
	class ViewContextFactory;
	struct FactorySettings;
	struct ContextSettings;
}
namespace Logging
{
	enum Severity;
}
}

class UCoherentBaseComponent;
class FCoherentUISystemListener;
class FCoherentUIFileHandler;
class FCoherentUIFactoryHolder;

class CoUISystemWrapper
{
public:
	~CoUISystemWrapper();

	static SystemWrapperSharedPtr CreateContext(Coherent::UI::FactorySettings& factorySettings, Coherent::UI::ContextSettings& settings, Coherent::Logging::Severity logSeverity);
	void DestroySystem(Coherent::UI::ViewContext* nakedContext);

	bool IsReady();

	void Update();
	void FetchSurfaces();
	void CreateView(UCoherentBaseComponent* owner, const Coherent::UI::ViewInfo& info, const wchar_t* path);
	void UnregisterComponent(UCoherentBaseComponent* component);
	void RegisterComponent(UCoherentBaseComponent* component);

	typedef TArray<TWeakObjectPtr<UCoherentBaseComponent>> WeakCoherentComponentsArray;
	WeakCoherentComponentsArray::TIterator GetCoherentComponentsIterator();

private:
	CoUISystemWrapper();

	TUniquePtr<FCoherentUISystemListener> Listener;
	TUniquePtr<FCoherentUIFileHandler> FileHandler;

	typedef TSharedPtr<FCoherentUIFactoryHolder, ESPMode::Fast> ViewContextFactoryPtr;
	typedef TWeakPtr<FCoherentUIFactoryHolder, ESPMode::Fast> ViewContextFactoryWeakPtr;


	Coherent::UI::ViewContext* Context;
	ViewContextFactoryPtr Factory;
	WeakCoherentComponentsArray WeakCoherentComponents;

	static ViewContextFactoryWeakPtr s_WeakFactory;
};
