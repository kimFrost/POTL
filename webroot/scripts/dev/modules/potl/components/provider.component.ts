///<reference path="../potl.module.ts"/>
///<reference path="../services/gamemode.service.ts"/>
///<reference path="../classes/resource.ts"/>
///<reference path="../classes/need.ts"/>
///<reference path="../classes/stat.ts"/>


namespace POTLModule {

	type GamemodeService = POTLModule.GamemodeService;

	export class ProviderController {

		private parent:StructureController;

		constructor(
			$rootScope: ng.IRootScopeService,
			$scope: ng.IScope,
			private $timeout: ng.ITimeoutService,
			private $interval: ng.IIntervalService,
			private $element: any, //IAugmentedJQuery
			private gamemodeService: GamemodeService
		) { }

		$onInit() {
			console.log('provider parent', this.parent);
		}

	}

	export class ProviderComponent implements ng.IComponentOptions {

		public bindings: any;
		public controller: any;
		public require: any;

		constructor() {
			this.require = {
				parent: '^structure'
			};
			this.bindings = {};
			this.controller = ProviderController;
		}
	}

	angular.module(POTLModule.moduleId).component("provider", new ProviderComponent());

}

