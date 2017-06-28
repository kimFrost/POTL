// Include a comment about why this seemingly unused module exists
var TemplateModule;
(function (TemplateModule) {
    angular.module('templates', []);
})(TemplateModule || (TemplateModule = {}));
var POTLModule;
(function (POTLModule) {
    var Resource = (function () {
        function Resource(ResourceId, Tags, Value, Locked, StoredIn) {
            if (ResourceId === void 0) { ResourceId = ''; }
            if (Tags === void 0) { Tags = []; }
            if (Value === void 0) { Value = 0; }
            if (Locked === void 0) { Locked = false; }
            this.ResourceId = ResourceId;
            this.Tags = Tags;
            this.Value = Value;
            this.Locked = Locked;
            this.StoredIn = StoredIn;
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
    var Need = (function () {
        function Need(Id, Title, Importance, Wants, Value) {
            if (Id === void 0) { Id = ''; }
            if (Title === void 0) { Title = ''; }
            if (Importance === void 0) { Importance = 0; }
            if (Wants === void 0) { Wants = []; }
            if (Value === void 0) { Value = 0; }
            this.Id = Id;
            this.Title = Title;
            this.Importance = Importance;
            this.Wants = Wants;
            this.Value = Value;
        }
        return Need;
    }());
    POTLModule.Need = Need;
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
///<reference path="../potl.module.ts"/>
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
        return GamemodeService;
    }());
    POTLModule.GamemodeService = GamemodeService;
    angular.module(POTLModule.moduleId).service("gamemodeService", GamemodeService);
})(POTLModule || (POTLModule = {}));
///<reference path="../potl.module.ts"/>
///<reference path="../services/gamemode.service.ts"/>
var POTLModule;
(function (POTLModule) {
    var StructureController = (function () {
        function StructureController($rootScope, $scope, $element, gamemodeService) {
            var _this = this;
            this.$element = $element;
            this.gamemodeService = gamemodeService;
            this.structuresInRange = [];
            this.location = {
                x: Number,
                y: Number
            };
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
            //$element[0].style.color = 'red';
            //$element[0].style.left = this.location.x + 'px';
            /*
            $element.css({
                'left': this.location.x + 'px',
                'top': this.location.y + 'px'
            });
            */
            $scope.$on('timeUpdate', function (e, arg) {
                //this.onTimeUpdate(arg...);
                _this.onTimeUpdate(arg.currentTime, arg.timeProgressed);
            });
        }
        StructureController.prototype.$onInit = function () {
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
                need.Value += timeProgressed;
            }
        };
        return StructureController;
    }());
    var StructureComponent = (function () {
        function StructureComponent() {
            this.bindings = {
                data: '<',
                location: '<'
                //onFacetChange: '&'
            };
            this.controller = StructureController;
            this.templateUrl = 'modules/potl/templates/structure.template.html';
            //this.template = '<div>fdgfdg</div>';
        }
        return StructureComponent;
    }());
    angular.module(POTLModule.moduleId).component("structure", new StructureComponent());
})(POTLModule || (POTLModule = {}));
