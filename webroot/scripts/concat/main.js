// Include a comment about why this seemingly unused module exists
var TemplateModule;
(function (TemplateModule) {
    angular.module('templates', []);
})(TemplateModule || (TemplateModule = {}));
var POTLModule;
(function (POTLModule) {
    var Feeling = (function () {
        function Feeling(Id, Title, ValueAddition, Affections, Convertions) {
            if (Id === void 0) { Id = ''; }
            if (Title === void 0) { Title = ''; }
            if (ValueAddition === void 0) { ValueAddition = 0; }
            if (Affections === void 0) { Affections = []; }
            if (Convertions === void 0) { Convertions = []; }
            this.Id = Id;
            this.Title = Title;
            this.ValueAddition = ValueAddition;
            this.Affections = Affections;
            this.Convertions = Convertions;
        }
        return Feeling;
    }());
    POTLModule.Feeling = Feeling;
    var Want = (function () {
        function Want(Id, Title, Target) {
            if (Id === void 0) { Id = ''; }
            if (Title === void 0) { Title = ''; }
            if (Target === void 0) { Target = ''; }
            this.Id = Id;
            this.Title = Title;
            this.Target = Target;
        }
        return Want;
    }());
    POTLModule.Want = Want;
    var Action = (function () {
        function Action(Id, Title) {
            if (Id === void 0) { Id = ''; }
            if (Title === void 0) { Title = ''; }
            this.Id = Id;
            this.Title = Title;
        }
        return Action;
    }());
    POTLModule.Action = Action;
    POTLModule.moduleId = "POTLModule";
    angular.module(POTLModule.moduleId, ['templates']);
})(POTLModule || (POTLModule = {}));
var POTLModule;
(function (POTLModule) {
    var Need = (function () {
        function Need(Id, Title, Importance, Wants, Value) {
            if (Id === void 0) { Id = ''; }
            if (Title === void 0) { Title = ''; }
            if (Importance === void 0) { Importance = 0; }
            if (Wants === void 0) { Wants = []; }
            if (Value === void 0) { Value = 50000; }
            this.Id = Id;
            this.Title = Title;
            this.Importance = Importance;
            this.Wants = Wants;
            this.Value = Value;
            this.Percentage = 100;
            this.MaxValue = 50000;
            this.Multiplier = 1;
        }
        Need.prototype.add = function (amount) {
            this.Value += amount;
            this.clampValue();
            this.updatePercentage();
        };
        Need.prototype.subtract = function (amount) {
            this.Value -= amount;
            this.clampValue();
            this.updatePercentage();
        };
        Need.prototype.clampValue = function () {
            if (this.Value < 0) {
                this.Value = 0;
            }
            else if (this.Value > this.MaxValue) {
                this.Value = this.MaxValue;
            }
            this.Value = Math.ceil(this.Value);
        };
        Need.prototype.updatePercentage = function () {
            this.Percentage = Math.ceil(this.Value / this.MaxValue * 100);
            return this.Percentage;
        };
        return Need;
    }());
    POTLModule.Need = Need;
})(POTLModule || (POTLModule = {}));
var POTLModule;
(function (POTLModule) {
    var Resource = (function () {
        function Resource(Id, Tags, MaxValue) {
            this.Id = '';
            this.Percentage = 100;
            this.Value = 0;
            this.Tags = [];
            this.MaxValue = 50000;
            this.Locked = false;
            this.Id = Id;
            this.Tags = Tags || this.Tags;
            this.MaxValue = MaxValue || this.MaxValue;
            this.Value = this.MaxValue;
        }
        Resource.prototype.GetOwner = function () {
        };
        Resource.prototype.Consume = function () {
            //void Consume(EConsumeType ConsumeType = EConsumeType::Undefined, bool bRemoveFromStorage = true);
        };
        Resource.prototype.RemoveFromStorage = function () {
        };
        Resource.prototype.Transfer = function () {
            //bool Transfer(UStorageComponent* Storage);
        };
        Resource.prototype.Init = function () {
        };
        return Resource;
    }());
    POTLModule.Resource = Resource;
})(POTLModule || (POTLModule = {}));
///<reference path="../potl.module.ts"/>
///<reference path="../components/structure.component.ts"/>
var POTLModule;
(function (POTLModule) {
    var GamemodeService = (function () {
        function GamemodeService($rootScope, $q, $interval, $window) {
            var _this = this;
            this.$rootScope = $rootScope;
            this.$q = $q;
            this.$interval = $interval;
            this.$window = $window;
            this.bFetchingFilters = false;
            this.currentTime = 0;
            this.playRate = 1;
            //private timeListeners: Array<ng.IDeferred<any>> = [];
            this.timeListeners = [];
            this.structures = [];
            $interval(function () {
                _this.tick(1000 / 60);
            }, 1000 / 60);
            // Tick should be on requestAnimationFrame
        }
        GamemodeService.prototype.progressTime = function (timeProgressed) {
            this.currentTime += timeProgressed;
            //OnTimeUpdated.Broadcast(Time, Amount);
            for (var _i = 0, _a = this.timeListeners; _i < _a.length; _i++) {
                var listener = _a[_i];
                listener(this.currentTime, timeProgressed);
            }
            this.$rootScope.$broadcast('timeUpdate', {
                currentTime: this.currentTime,
                timeProgressed: timeProgressed
            });
        };
        GamemodeService.prototype.tick = function (deltaTime) {
            this.progressTime(deltaTime * this.playRate);
        };
        GamemodeService.prototype.bindToTimeUpdate = function (caller, method) {
            this.timeListeners.push(method);
        };
        GamemodeService.prototype.registerStructure = function (structure) {
            if (structure) {
                this.structures.push(structure);
                this.$rootScope.$broadcast('structureRegistered', structure);
            }
        };
        GamemodeService.prototype.getStructures = function () {
            return this.structures;
        };
        return GamemodeService;
    }());
    POTLModule.GamemodeService = GamemodeService;
    angular.module(POTLModule.moduleId).service("gamemodeService", GamemodeService);
})(POTLModule || (POTLModule = {}));
///<reference path="../potl.module.ts"/>
///<reference path="../services/gamemode.service.ts"/>
///<reference path="../classes/resource.ts"/>
///<reference path="../classes/need.ts"/>
var POTLModule;
(function (POTLModule) {
    var StructureController = (function () {
        function StructureController($rootScope, $scope, $element, //IAugmentedJQuery
            gamemodeService) {
            var _this = this;
            this.$element = $element;
            this.gamemodeService = gamemodeService;
            this.structuresInRange = [];
            this.location = {
                x: 0,
                y: 0
            };
            this.id = '';
            this.needPreferences = [
                {
                    Id: "NEED_Food",
                    MinValue: 10
                },
                {
                    Id: "NEED_Sleep",
                    MinValue: 20
                },
            ];
            this.Segments = {
                0: {
                    Min: 0,
                    Max: 10
                },
                1: {
                    Min: 10,
                    Max: 30
                }
            };
            this.needs = [];
            this.wants = [];
            this.resources = [];
            $scope.$on('timeUpdate', function (e, arg) {
                //this.onTimeUpdate(arg...);
                _this.onTimeUpdate(arg.currentTime, arg.timeProgressed);
            });
            $scope.$on('structureRegistered', function (e, structure) {
                if (structure && structure !== _this) {
                    _this.checkStructureInRange(structure);
                }
            });
        }
        StructureController.prototype.$onInit = function () {
            console.log('mind actor init');
            this.$element[0].style.left = this.location.x + 'px';
            this.$element[0].style.top = this.location.y + 'px';
            this.gamemodeService.registerStructure(this);
            if (this.id.toString() === '1') {
                var resource = new POTLModule.Resource('Apples', ['Food']);
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
            var need = new POTLModule.Need('NEED_Food');
            this.needs.push(need);
            //this.mindService.bindToTimeUpdate(this, this.onTimeUpdate);
            // Strive for emotional balance.
        };
        StructureController.prototype.onTimeUpdate = function (time, timeProgressed) {
            //console.log('onTimeUpdate', time, timeProgressed);
            this.time = time;
            this.progressNeeds(timeProgressed);
        };
        StructureController.prototype.progressNeeds = function (timeProgressed) {
            for (var _i = 0, _a = this.needs; _i < _a.length; _i++) {
                var need = _a[_i];
                //need.Wants
                need.subtract(timeProgressed);
                //need.Value += timeProgressed;
                // If not anything in area to meet need, then subtract 
            }
        };
        StructureController.prototype.checkStructureInRange = function (structure) {
            if (structure) {
                var distanceX = Math.abs(this.location.x - structure.location.x);
                var distanceY = Math.abs(this.location.y - structure.location.y);
                var distance = Math.sqrt(Math.pow(distanceX, 2) + Math.pow(distanceY, 2));
                if (distance < 500) {
                    this.connectToStructure(structure);
                    structure.connectToStructure(this);
                }
            }
        };
        StructureController.prototype.connectToStructure = function (structure) {
            console.log(this.structuresInRange.indexOf(structure));
            if (structure && this.structuresInRange.indexOf(structure) === -1) {
                this.structuresInRange.push(structure);
            }
        };
        return StructureController;
    }());
    POTLModule.StructureController = StructureController;
    var StructureComponent = (function () {
        function StructureComponent() {
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
        return StructureComponent;
    }());
    POTLModule.StructureComponent = StructureComponent;
    angular.module(POTLModule.moduleId).component("structure", new StructureComponent());
})(POTLModule || (POTLModule = {}));
