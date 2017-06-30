///<reference path="../potl.module.ts"/>
///<reference path="../services/gamemode.service.ts"/>
///<reference path="../classes/resource.ts"/>
///<reference path="../classes/need.ts"/>
///<reference path="../classes/stat.ts"/>


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
		public id: string = '';

		public needs: Array<Need>;
		public wants: Array<IWant>;
		public resources: Array<Resource>;

		public wealth: Stat;

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
			private $timeout: ng.ITimeoutService,
			private $interval: ng.IIntervalService,
			private $element: any, //IAugmentedJQuery
			private gamemodeService: GamemodeService
		) {
			this.needs = [];
			this.wants = [];
			this.resources = [];
			this.wealth = new Stat('wealth');

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

			/*
			if (this.id.toString() === '1') {
				let resource = new Resource('Apples', 'Apples', ['Food']);
				this.resources.push(resource);
			}
			*/

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
			let need = new Need('NEED_Food', 'Food');
			this.needs.push(need);

/*
			this.$interval(() => {
				if (this.id.toString() === '1') {
					let resource = new Resource('Apples', 'Apples', ['Food']);
					this.resources.push(resource);
				}
			}, 35000);
			*/

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

				let needDegradation: number = timeProgressed;

				let resource = this.requestResourceByTag('Food', needDegradation);
				if (resource) {
					if (resource.Value > needDegradation) {
						resource.subtract(needDegradation);
						needDegradation = 0;
					}
					else {
						needDegradation -= resource.Value;
						resource.subtract(resource.Value);
					}
				}
				else {
					for (let structure of this.structuresInRange) {
						if (needDegradation > 0) {
							let resource = structure.requestResourceByTag('Food');
							if (resource) {
								if (resource.Value > needDegradation) {
									resource.subtract(needDegradation);
									structure.wealth.add(needDegradation * resource.PricePerUnit);
									this.wealth.subtract(needDegradation * resource.PricePerUnit);
									needDegradation = 0;
								}
								else {
									needDegradation -= resource.Value;
									structure.wealth.add(resource.Value * resource.PricePerUnit);
									this.wealth.subtract(resource.Value * resource.PricePerUnit);
									resource.subtract(resource.Value);
								}
							}
						}
						else {
							break;
						}
					}
				}

				// If not anything in area to meet need, then subtract 
				if (needDegradation > 0) {
					need.subtract(timeProgressed);
				}
				else if (needDegradation < 0) {
					need.subtract(timeProgressed);
				}
			}
		}

		public requestResourceByTag(tag: string, amount?: number): Resource {
			for (let resource of this.resources) {
				if (resource.Tags.indexOf('Food') !== -1 && resource.Value > 0) {
					return resource;
				}
			}
			return null;
		}

		private checkStructureInRange(structure: StructureController): void {
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

		public connectToStructure(structure: StructureController): void {
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
		public transclude: any;

		constructor() {
			this.bindings = {
				data: '<',
				location: '<',
				id: '<'
				//onFacetChange: '&'
			};
			
			this.transclude = true;
			/*
			this.transclude = {
				components: 'components'
			};
			*/
			
			this.controller = StructureController;
			this.templateUrl = 'modules/potl/templates/structure.template.html';
			//this.template = '<div>fdgfdg</div>';
		}
	}

	angular.module(POTLModule.moduleId).component("structure", new StructureComponent());

}

