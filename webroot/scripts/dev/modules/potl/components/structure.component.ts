///<reference path="../potl.module.ts"/>
///<reference path="../services/gamemode.service.ts"/>
///<reference path="../classes/resource.ts"/>
///<reference path="../classes/need.ts"/>


namespace POTLModule {

	type GamemodeService = POTLModule.GamemodeService;

	export class StructureController {

		public data: any;
		//public onFacetChange:Function;
		public time: number;

		public structuresInRange: Array<StructureController> = [];
		public location = {
			x: 0,
			y: 0
		};
		public id:string = '';

		public needs: Array<Need>;
		public wants: Array<IWant>;
		public resources: Array<Resource>;

		public needPreferences = [
			{
				Id: "NEED_Food",
				MinValue: 10
			},
			{
				Id: "NEED_Sleep",
				MinValue: 20
			},
		];
		public Segments = {
			0: {
				Min: 0,
				Max: 10,
			},
			1: {
				Min: 10,
				Max: 30,
			}
		};


		constructor(
			$rootScope: ng.IRootScopeService,
			$scope: ng.IScope,
			private $element: any, //IAugmentedJQuery
			private gamemodeService: GamemodeService
		) {
			this.needs = [];
			this.wants = [];
			this.resources = [];

			$scope.$on('timeUpdate', (e: any, arg: any) => {
				//this.onTimeUpdate(arg...);
				this.onTimeUpdate(arg.currentTime, arg.timeProgressed);
			});

			$scope.$on('structureRegistered', (e: any, structure: any) => {
				if (structure && structure !== this) {
					this.checkStructureInRange(structure);
				}
			});
		}

		$onInit() {
			console.log('mind actor init');

			this.$element[0].style.left = this.location.x + 'px';
			this.$element[0].style.top = this.location.y + 'px';

			this.gamemodeService.registerStructure(this);

			if (this.id.toString() === '1') {
				let resource = new Resource('Apples', ['Food']);
				this.resources.push(resource);
			}

			/*
			let need = {
				type: 'eat',
				level: 1,
				wants: [
					{
						targetTags: ['food']
					}
				]
			}
			*/
			let need = new Need('NEED_Food');
			this.needs.push(need);

			//this.mindService.bindToTimeUpdate(this, this.onTimeUpdate);

			// Strive for emotional balance.

		}

		public onTimeUpdate(time: number, timeProgressed: number): void {
			//console.log('onTimeUpdate', time, timeProgressed);
			this.time = time;
			this.progressNeeds(timeProgressed);
		}

		public progressNeeds(timeProgressed: number): void {
			for (let need of this.needs) {
				//need.Wants
				need.subtract(timeProgressed);
				//need.Value += timeProgressed;

				// If not anything in area to meet need, then subtract 

			}

			
		}

		private checkStructureInRange(structure:StructureController):void {
			if (structure) {
				let distanceX = Math.abs(this.location.x - structure.location.x);
				let distanceY = Math.abs(this.location.y - structure.location.y);
				let distance = Math.sqrt(Math.pow(distanceX, 2) + Math.pow(distanceY, 2));
				if (distance < 500) {
					this.connectToStructure(structure);
					structure.connectToStructure(this);
				}
			}
		}

		public connectToStructure(structure:StructureController):void {
			console.log(this.structuresInRange.indexOf(structure));
			if (structure && this.structuresInRange.indexOf(structure) === -1) {
				this.structuresInRange.push(structure);
			}
		}

	}

	export class StructureComponent implements ng.IComponentOptions {

		public bindings: any;
		public controller: any;
		//public template: string;
		public templateUrl: string;

		constructor() {
			this.bindings = {
				data: '<',
				location: '<',
				id: '<'
				//onFacetChange: '&'
			};
			this.controller = StructureController;
			this.templateUrl = 'modules/potl/templates/structure.template.html';
			//this.template = '<div>fdgfdg</div>';
		}
	}

	angular.module(POTLModule.moduleId).component("structure", new StructureComponent());

}

