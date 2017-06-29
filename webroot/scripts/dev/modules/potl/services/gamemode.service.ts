///<reference path="../potl.module.ts"/>
///<reference path="../components/structure.component.ts"/>


namespace POTLModule {

    export class GamemodeService {

        public bFetchingFilters: boolean = false;
        public currentTime: number = 0;
        private playRate: number = 1;
        //private timeListeners: Array<ng.IDeferred<any>> = [];
        private timeListeners: Array<Function> = [];
        private timeSinceLastTick: number;
        private structures:Array<StructureController> = [];

        public progressTime(timeProgressed: number): void {
            this.currentTime += timeProgressed;
            //OnTimeUpdated.Broadcast(Time, Amount);
            for (let listener of this.timeListeners) {
                listener(this.currentTime, timeProgressed);
            }
            this.$rootScope.$broadcast('timeUpdate', {
                currentTime: this.currentTime,
                timeProgressed: timeProgressed
            });
        }

        public tick(deltaTime: number): void {
            this.progressTime(deltaTime * this.playRate);
        }

        public bindToTimeUpdate(caller: any, method: Function) {
            this.timeListeners.push(method);
        }

        public registerStructure(structure:StructureController) {
            if (structure) {
                this.structures.push(structure);
                this.$rootScope.$broadcast('structureRegistered', structure);
            }
        }

        public getStructures():Array<StructureController> {
            return this.structures;
        }

        constructor(
            private $rootScope: ng.IRootScopeService,
            private $q: ng.IQService,
            private $interval: ng.IIntervalService,
            private $window: ng.IWindowService
        ) {
            $interval(() => {
                this.tick(1000 / 60);
            }, 1000 / 60);
            // Tick should be on requestAnimationFrame
        }
    }

    angular.module(POTLModule.moduleId).service("gamemodeService", GamemodeService);

}