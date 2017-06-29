///<reference path="../references/references.ts"/>

namespace MainApp {
	export function appConfig(
		$stateProvider,
		$locationProvider,
		$urlRouterProvider,
		$uiViewScrollProvider,
		$httpProvider,
		localStorageServiceProvider) {

		$urlRouterProvider.otherwise("/");

		localStorageServiceProvider
			.setPrefix('nemligApp')
			.setStorageType('sessionStorage');

		$locationProvider.html5Mode({
			enabled: Modernizr.history, //Active html5 mode only if browser supports it (IE9 does page-load on navigation)
			requireBase: false,
			rewriteLinks: true
		});

		$stateProvider.state('root', {
			url: '*path',

			resolve: {
				pagedata: function ($stateParams, contentService: PageModule.ContentService) {
					return contentService.getItem($stateParams.path);
				}
			},
			templateProvider: templateProvider,
			controller: function ($scope, pagedata) {
				$scope.pagedata = pagedata;

				// Hide full page overlay
			}
		});

		function templateProvider(pagedata, contentService: PageModule.ContentService) {
			var templateName = contentService.getTemplateIdentifier(pagedata.MetaData.TemplateId);
			var template = "<" + templateName + " pagedata='pagedata'></" + templateName + ">";
			return template;
		}

		$httpProvider.interceptors.push('httpInterceptor');

		// Make the uiView autoscroll, scroll to top of page and not just the ui-view
		$uiViewScrollProvider.useAnchorScroll();

	}
}
