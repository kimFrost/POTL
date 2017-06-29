
// ===================================================
// Grunt plugins
// ===================================================

//
// grunt
// https://github.com/gruntjs
//
// grunt-browser-sync /* REMOVE */
//
// grunt-cli
// https://github.com/gruntjs/grunt-cli
//
// grunt-contrib-clean  (Clean folders, delete files)
// https://github.com/gruntjs/grunt-contrib-clean
//
// grunt-contrib-concat (Concat files)
// https://github.com/gruntjs/grunt-contrib-concat
//
// grunt-contrib-copy (Copy files)
// https://github.com/gruntjs/grunt-contrib-copy
//
// grunt-contrib-imagemin (Image compression)
// https://github.com/gruntjs/grunt-contrib-imagemin
//
// grunt-contrib-uglify (Minify js)
// https://github.com/gruntjs/grunt-contrib-uglify
//
// grunt-contrib-watch (Watch file changes, trigger shit)
// https://github.com/gruntjs/grunt-contrib-watch
//
// grunt-newer
// https://github.com/tschaub/grunt-newer
//
// grunt-ng-annotate
// https://github.com/mgol/grunt-ng-annotate
//
// grunt-postcss
// https://github.com/postcss/postcss
//
// Autoprefixer (Autoprefix vendor prefixes. Use with postcss)
// https://github.com/postcss/autoprefixer
//
// grunt-sass
// https://github.com/sindresorhus/grunt-sass
//
// grunt-styledown
// https://github.com/drakeh/grunt-styledown
//
// grunt-ts
// https://github.com/TypeStrong/grunt-ts
//
// grunt-tslint
// https://github.com/palantir/grunt-tslint
//
// jit-grunt (conditionally load/require node modules)
// https://github.com/shootaroo/jit-grunt
//


// REMOVE ONE BY ONE TO SEE IF USED
// grunt-datauri /* REMOVE */
// ng-annotate /* REMOVE */
// https://github.com/olov/ng-annotate
//
// node-bourbon /* REMOVE */
//
// grunt-spritesmith
//
// sorcery /* ?????? REMOVE ?????? */
// https://github.com/Rich-Harris/sorcery
//
// tslint /* REMOVE */
// https://github.com/palantir/tslint


module.exports = function (grunt) {

	// Relative paths (not all configs can use these (object keys can't be variables) - check everything works if you change the paths)

	var relativePath = "SCommerce.Website/SCom/",
		distPath = relativePath + "dist/",

		devScriptsPath = relativePath + "scripts/dev/",
		scriptsTempPath = relativePath + "scripts/temp/",
		jsDistPath = distPath + "js/",

		scssPath = relativePath + "scss/",
		cssPath = relativePath + "css/",
		cssDistPath = distPath + "css/",

		imagesPath = relativePath + "images/",
		imagesDistPath = distPath + "images/",

		fontsPath = relativePath + "fonts/",
		fontsDistPath = distPath + "fonts/",

		speakPath = relativePath + "components/speak",
		speakDistPath = distPath + "speak/",

		cshtmlPath = relativePath + "renderings/",

		styleGuidePath = relativePath + "Styleguide/";

	// Load modules "Just-In-Time"
	require("jit-grunt")(grunt, {
		sprite: "grunt-spritesmith" // Statically bind to sprite task.
	});

	// Import ng-annotate for use in requirejs onBuildRead callback + Array of valid annotation paths
	var ngAnnotate = require("ng-annotate"),
	annotateModuleRegex = /(ngModules).*(\/config\/|\/controllers\/|\/directives\/|\/services\/|\/filters\/)/,
	hasAnnotationErrors = false;


	// Project configuration
	grunt.initConfig({

		// copy task
		// copies folder and files to dist folder
		copy: {
			images: {
				files: [
					{
						expand: true,
						cwd: imagesPath,
						src: ['**/*'],
						dest: imagesDistPath
					}
				]
			},
			fonts: {
				files: [
					{
						expand: true,
						cwd: fontsPath,
						src: ['**/*'],
						dest: fontsDistPath
					}
				]
			},
			//This is for javascript files in the SPEAK modules in Sitecore
			speak: {
				files: [
					{
						expand: true,
						cwd: speakPath,
						src: ['AdvancedRecipeGroupSearch/*', 'ElevateSearchSettingsEditor/*', 'FiltersEditor/*', 'ProductsSelector/*', 'RecipePreview/*', 'RecipeProductsSelector/*', 'RecipeRecommendationsSelector/*', 'SearchBoosterSettingsEditor/*', 'SearchSynonyms/*'],
						dest: speakDistPath
					}
				]
			}
		},

		// clean task
		clean: {
		    all: [distPath + "*", scriptsTempPath + "*"],
			css: [cssDistPath],
			fonts: [fontsDistPath],
			images: [imagesDistPath],
			speak: [speakDistPath],
			js: [jsDistPath],
			jstemp: [scriptsTempPath + "*"]
		},

		/* grunt-sass task */
		sass: {
			// Default options
			options: {
				imagePath: imagesPath,
				// Path to images folder. Allows the use of "image-url('image.png')", without the need to write the entire path.
				outputStyle: "nested" //Expand css
			},
			// All non-include (e.g. _include.scss) files need to be defined here
			files: {
				"SCommerce.Website/SCom/css/common.css": scssPath + "common.scss",
				"SCommerce.Website/SCom/css/experience-editor.css": scssPath + "experience-editor.scss",
				"SCommerce.Website/SCom/css/dibs.css": scssPath + "dibs.scss",
			},
			// For local and test
			dev: {
				options: {
					sourceMap: true, // Might be slow
				},
				// All non-include (e.g. _include.scss) files need to be defined here (REMEMBER TO INCLUDE ANY NEW FILES IN POSTCSS CONFIG)
				files: {
					"SCommerce.Website/SCom/css/common.css": scssPath + "common-dev.scss",
					"SCommerce.Website/SCom/css/experience-editor.css": scssPath + "experience-editor.scss",
					"SCommerce.Website/SCom/css/dibs.css": scssPath + "dibs.scss",
				}
			},
			// For staging and production
			dist: {
				// Override default options
				options: {
					sourceMap: false, 
					outputStyle: "compressed" //Compress css
				},
				// All non-include (e.g. _include.scss) files need to be defined here (REMEMBER TO INCLUDE ANY NEW FILES IN POSTCSS CONFIG)
				files: {
					"SCommerce.Website/SCom/css/common.css": scssPath + "common.scss",
					"SCommerce.Website/SCom/css/experience-editor.css": scssPath + "experience-editor.scss",
					"SCommerce.Website/SCom/css/dibs.css": scssPath + "dibs.scss",
				}
			}
		},
		/* grunt-postcss task */
		postcss: {
			options: {
				// PostCSS plugins
				processors: [
					// Use autoprefixer plugin
					require("autoprefixer")({
						// Specify supported browsers: https://github.com/ai/browserslist
						browsers: [
							// Desktop:
							"> 1%", // Usage greater than 1% by global usage statistics
							"last 2 versions", // The last 2 versions for each major browser
							"Firefox ESR", // FireFox Extended Support Release
							"Opera >= 12", // Opera 12 and up
							"IE >= 9", // IE9 and up
							// Mobile:
							"Android >= 2.3", // Android mobile 2.3 and up
							"iOS >= 4", // Safari mobile 4 and up
							"FirefoxAndroid >= 33", // FF for android 33 and up
							"ChromeAndroid >= 40", // Chrome for android 40 and up
							"OperaMobile >= 12", // Opera mobile 12 and up
							"ExplorerMobile >= 10" // IE mobile 10 and up
						]
					})
				]
			},
			dev: {
				options: {
					map: true
				}
			},
			dist: {
				options: {
					map: false
				},
				src: cssPath + "common.css",
				dest: cssDistPath + "common.css"
			}
		},

		/* grunt-tslint task */
		tslint: {
			options: {
				configuration: grunt.file.readJSON("tslint.json")
			},
			files: {
				src: [
					devScriptsPath + "**/*.ts"
				]
			}
		},

		/* grunt-ts task */
		ts: {
			all: {
				options: {
					target: "es5", // Target EcmaScript 5
					sourceMap: true, // Generate a source map for every output js file. [true (default) | false]
					comments: true, // Preserve comments for @ngInject usage
					htmlModuleTemplate: "HtmlTemplates.<%= filename %>",
					htmlOutDir: scriptsTempPath,
					htmlOutDirFlatten: true,
					fast: "never" // Full compile
				},
				out: jsDistPath + "scripts.js",
				html: devScriptsPath + "**/*.html",
				src: [
					"typings/browser.d.ts",
					relativePath + "scripts/references/translations/TranslationTemplate.d.ts",
					relativePath + "scripts/references/typelite/TypeLite.Net4.d.ts",
					relativePath + "scripts/references/customTypings/angular-cookie.d.ts",
					scriptsTempPath + "**/*.html.ts",
					devScriptsPath + "global/**/*.ts",
					devScriptsPath + "app.config.ts",
					devScriptsPath + "app.run.ts",
					devScriptsPath + "components/**/*.module.ts",
					devScriptsPath + "components/**/*.provider.ts",
					devScriptsPath + "components/**/*.config.ts",
					devScriptsPath + "components/**/*.ts",
					devScriptsPath + "!components/**/*.html.ts",
					devScriptsPath + "!components/**/*.spec.ts",
					devScriptsPath + "app.ts"
				],
				reference: relativePath + "scripts/references/references.ts"
			}
		},

		concat_sourcemap: {
			dist: {
				// The files to concatenate
				src: [
					relativePath + "scripts/3rdParty/hammerjs/hammer.min.js",
					relativePath + "scripts/3rdParty/**/*.js",
					jsDistPath + "scripts.js" // compiled .ts files
				],
				dest: jsDistPath + "main.js"
			}
		},

		/* grunt-contrib-uglify task */
		uglify: {
			options: {
				sourceMap: false,
				mangle: false, // Must be false, to work with ngAnnotate
				preserveComments: false
			},
			dist: {
				src: jsDistPath + "main.js",
				dest: jsDistPath + "main.min.js"
			}
		},

		/* grunt-ng-annotate task */
		ngAnnotate: {
			options: {
				singleQuotes: true
			},
			dist: {
				files: [
					{
						expand: true,
						src: [jsDistPath + "main.js"],
						ext: ".js", // Dest filepaths will have this extension.
						extDot: "last" // Extensions in filenames begin after the last dot
					}
				]
			}
		},

		/* grunt-styledown task */
		styledown: {
			build: {
				files: {
					"SCommerce.Website/SCom/Styleguide/index.html": [styleGuidePath + "markdown/*.md"]
				},
				options: {
					config: styleGuidePath + "config/config.md"
				}
			}

		},
		/* grunt-contrib-watch task */
		watch: {
			configFiles: {
				files: ["Gruntfile.js"],
				options: {
					reload: true
				}
			},
			options: {
				maxListeners: 1000, // Increase maxListeners to avoid errors
				livereload: grunt.option('reload') || {
					host: "localhost"
				}
			},

			// Watch ts/js files
			scripts: {
				files: [
					relativePath + "scripts/references/references.ts",
					devScriptsPath + "**/*.ts",
					devScriptsPath + "**/*.html",
					"!" + devScriptsPath + "**/*.html.ts"
				],
				tasks: ["allscripts"],
				options: {
					spawn: false // Don't spawn child processes
				}
			},

			// Watch scss files
			scss: {
				files: [scssPath + "**/*.scss"],
				tasks: ["allcss"],
				options: {
					spawn: false // Don't spawn child processes
				}
			},

			// Watch Styleguide files
			styledown: {
				files: [styleGuidePath + "**/*.md"],
				tasks: ["styledown"],
				options: {
					spawn: false // Don't spawn child processes
				}
			},

			// Watch renderings
			cshtml: {
				files: [cshtmlPath + "**/*.cshtml"],
				options: {
					spawn: false // Don't spawn child processes
				}
			},

			// Watch images
			images: {
				files: [imagesPath + "**/*"],
				tasks: ["newer:copy:images"],
				options: {
					spawn: false // Don't spawn child processes
				}
			},

			// Watch fonts
			fonts: {
				files: [fontsPath + "**/*"],
				tasks: ["newer:copy:fonts"],
				options: {
					spawn: false // Don't spawn child processes
				}
			}
		}
	});


	// Custom Grunt tasks
	// =============================================
	/* Default grunt task (runs on "grunt" execution i.e. no task specified) */
	grunt.registerTask("builddev", "Default grunt task (runs on \"grunt\" execution i.e. no task specified)",
	["clean:all", "tslint", "ts:all", "concat_sourcemap", "sass:dev", "postcss", "copy", "styledown"]);

	grunt.registerTask("buildstaging", "Default grunt task (runs on \"grunt\" execution i.e. no task specified)",
	["clean:all", "tslint", "ts:all", "concat_sourcemap", "sass:dist", "postcss", "copy", "styledown", "uglify"]);


	// Action specific tasks
	// =============================================
	// Clean-all task
	grunt.registerTask("cleanall", "Makes clean:all default clean task, and runs that one only",
		["clean:all"]);

	// Run all SASS and CSS related tasks
	grunt.registerTask("allcss", "Run all SASS and CSS related tasks",
	   ["sass:dev", "postcss"]);

	// Run all TS and JS related tasks
	grunt.registerTask("allscripts", "Run all TS and JS related tasks",
	   ["ts:all", "concat_sourcemap"]);

	// Run all TS and JS related tasks
	grunt.registerTask("allimages", "Run imagemin task on all images",
	   ["imagemin"]);


	// Development/Local tasks
	// =============================================
	/* Runs on git push from local machine - used to validate TypeScript before pushing to repository */
	grunt.registerTask("gitpush", "Runs on git push from local machine - used to validate TypeScript before pushing to repository",
		["tslint"]);

	/* Runs on MSBuild via Grunt.targets if solution is in "Debug" mode */
	grunt.registerTask("default", "Runs on MSBuild via Grunt.targets if solution is in \"Debug\" mode",
		["builddev", "watch"]);

	/* Runs on MSBuild via Grunt.targets if solution is in "Debug" mode */
	grunt.registerTask("local", "Runs on MSBuild via Grunt.targets if solution is in \"Debug\" mode",
		["builddev"]);


	// Deploy/TeamCity tasks
	// =============================================
	/* Runs on MSBuild via Grunt.targets if solution is in "Gennemse" mode */
	grunt.registerTask("dev", "Runs on MSBuild via Grunt.targets if solution is in \"Gennemse\" mode",
		["builddev"]);

	/* Runs on MSBuild via Grunt.targets if solution is in "Godkend" mode */
	grunt.registerTask("staging", "Runs on MSBuild via Grunt.targets if solution is in \"Godkend\" mode",
		["buildstaging"]);

	/* Runs on MSBuild via Grunt.targets if solution is in "Release/Frigiv" mode */
	grunt.registerTask("deploy", "Runs on MSBuild via Grunt.targets if solution is in \"Release/Frigiv\" mode",
		["buildstaging"]);

};
