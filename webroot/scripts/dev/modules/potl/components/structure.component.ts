///<reference path="../potl.module.ts"/>
///<reference path="../services/gamemode.service.ts"/>


namespace POTLModule {

	type GamemodeService = POTLModule.GamemodeService;

	class StructureController {

		public data: any;
		//public onFacetChange:Function;
		public time: number;
		
		public structuresInRange:Array<StructureComponent> = [];
		public location = {
			x:Number,
			y:Number
		};

		public needs: Array<INeed>;
		public wants: Array<IWant>;

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
			private $element: ng.IAugmentedJQuery,
			private gamemodeService: GamemodeService
		) {
			this.needs = [];
			this.wants = [];

			//$element[0].style.color = 'red';
			
			//$element[0].style.left = this.location.x + 'px';

			/*
			$element.css({
				'left': this.location.x + 'px',
				'top': this.location.y + 'px'
			});
			*/

			$scope.$on('timeUpdate', (e: any, arg: any) => { 
				//this.onTimeUpdate(arg...);
				this.onTimeUpdate(arg.currentTime, arg.timeProgressed);
			});
		}

		$onInit() {
			console.log('mind actor init');

			this.$element[0].style.left = this.location.x + 'px';
			this.$element[0].style.top = this.location.y + 'px';
			
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
				need.Value += timeProgressed;
			}


		}

	}

	class StructureComponent implements ng.IComponentOptions {

		public bindings: any;
		public controller: any;
		//public template: string;
		public templateUrl: string;

		constructor() {
			this.bindings = {
				data: '<',
				location: '<'
				//onFacetChange: '&'
			};
			this.controller = StructureController;
			this.templateUrl = 'modules/potl/templates/structure.template.html';
			//this.template = '<div>fdgfdg</div>';
		}
	}

	angular.module(POTLModule.moduleId).component("structure", new StructureComponent());

}

