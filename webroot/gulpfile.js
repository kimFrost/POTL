'use strict';

// Include Gulp & tools we'll use
var gulp = require('gulp');
var $ = require('gulp-load-plugins')();
var browserSync = require('browser-sync').create();
var reload = browserSync.reload;
var del = require('del');
var runSequence = require('run-sequence');
var wiredep = require('wiredep').stream;
var jade = require('gulp-jade');
var ts = require('gulp-typescript');
var templateCache = require('gulp-angular-templatecache');
var concat = require('gulp-concat');

var tsProject = ts.createProject('tsconfig.json', {});


var AUTOPREFIXER_BROWSERS = [
    'ie >= 10',
    'ie_mob >= 10',
    'ff >= 30',
    'chrome >= 34',
    'safari >= 7',
    'opera >= 23',
    'ios >= 7',
    'android >= 4.4',
    'bb >= 10'
];

var paths = {
    scripts: 'scripts/**/*.js',
    images: 'images/**/*',
};

// Clean output directory
gulp.task('clean', del.bind(null, ['../css/*', '../images/**/*', '../scripts/*'], { dot: true, force: true }));

// Lint JavaScript
gulp.task('jshint', function () {
    return gulp.src('scripts/**/*.js')
        .pipe($.jshint())
        .pipe($.jshint.reporter('jshint-stylish'))
        .pipe(reload({ stream: true }));
});

// Lint Sass
gulp.task('scss-lint', function () {
    return gulp.src('scss/*.scss')
        .pipe($.scssLint({
            'config': 'scss-lint.yml'
        }));
});

// Compile and automatically prefix stylesheets
gulp.task('styles', function () {
    // For best performance, don't add Sass partials to `gulp.src`
    return gulp.src([
        'scss/*.scss'
    ])
        .pipe($.sourcemaps.init())
        .pipe($.sass({
            precision: 10,
            onError: console.error.bind(console, 'Sass error:')
        }))
        .pipe($.autoprefixer({ browsers: AUTOPREFIXER_BROWSERS }))
        .pipe($.sourcemaps.write())
        .pipe(gulp.dest('./dist/css'))
        .pipe($.size({ title: 'styles' }))
        .pipe(reload({ stream: true }));
});

// Auto inject bower-components
gulp.task('bower', function () {
    return gulp.src('../Views/_Master.cshtml')
        .pipe(wiredep({
            exclude: [/es5-shim/, /json3/, /bootstrap-sass/, /normalize-css/, /jquery/]
        }))
        .pipe(gulp.dest('../Views'));
});

// Reads HTML for usemin blocks to enable smart deploys that automatically concats files
gulp.task('usemin', function () {
    return gulp.src('../Views/_Master.cshtml')
        .pipe($.usemin({
            css: [$.csso()],
            jsLibs: [$.uglify()],
            jsMain: [$.ngAnnotate(), $.uglify()]
        }))
        .pipe(gulp.dest('../'));
});

// Browser-sync
gulp.task('browser-sync', function () {
    return browserSync.init({
        //proxy: "http://localhost:49746/"
        server: {
            baseDir: "./dist/"
        }
    });
});

// Copy images
gulp.task('copyImages', function () {
    return gulp.src(['images/**/*'], { base: '.' })
        .pipe(gulp.dest('../'))
        .pipe($.size({ title: 'images' }));
});

// Optimize images
gulp.task('optimizeImages', function () {
    return gulp.src(['images/**/*'], { base: '.' })
        .pipe($.imagemin({
            progressive: true,
            optimizationLevel: 5
        }))
        .pipe(gulp.dest('../'))
        .pipe($.size({ title: 'images' }));
});

// Copy Templates
gulp.task('templates', function () {
    return gulp.src(['templates/**/*'], { base: '.' })
        .pipe(gulp.dest('../'))
        .pipe($.size({ title: 'templates' }))
        .pipe(reload({ stream: true }));
});

// Render jade files
gulp.task('renderViews', function () {
    return gulp.src('views/pages/*.jade')
        .pipe(jade({
            client: false
        }))
        .pipe(gulp.dest('./dist'))
        .pipe(reload({ stream: true }));
});

gulp.task('templateCache', function () {
    return gulp.src('scripts/dev/**/*.html')
        .pipe(templateCache({
            //module: 'app.templates',
            standalone: false,
            //root: '/'
            //base: '/base'
        }))
        //.pipe(concat('templates.js'))
        .pipe(gulp.dest('scripts/concat'));
});

gulp.task('ts', function () {
    return gulp.src('scripts/**/*.ts')
        .pipe(ts({
            noImplicitAny: true,
            outFile: 'main.js'
            //reference: 'tmp/references.ts'
        }))
        .pipe(gulp.dest('scripts/concat'));
});

gulp.task('concat', function () {
    return gulp.src('scripts/concat/*.js')
        .pipe(concat('main.js'))
        .pipe(gulp.dest('./dist/'))
        .pipe(reload({ stream: true }));
});

gulp.task('ts2', function () {
    /*
    return tsProject.src()
            .pipe(sourcemaps.init())
            .pipe(ts(tsProject))
            .pipe(sourcemaps.write())
            .pipe(gulp.dest(options.distES6));
    */
    return tsProject.src()
        .pipe(tsProject())
        //.js.pipe(gulp.dest("dist/scripts/main.js"));
        //.pipe(gulp.dest("dist/scripts/*"));
        .pipe(gulp.dest("./dist/scripts"));
});


// Rerun the task when a file changes
gulp.task('watch', function () {
    gulp.watch('scripts/dev/**/*.ts', function () { runSequence('ts', 'concat') });
    gulp.watch('images/**/*', ['copyImages']);
    gulp.watch('scss/**/*.scss', ['styles']);
    gulp.watch('templates/**/*', ['templates']);
    gulp.watch('scripts/dev/**/*.html', function () { runSequence('templateCache', 'concat') });
    gulp.watch('views/**/*', ['renderViews']);
});

//gulp.watch('scripts/**/*.js', ['jshint']);





// Build production files, the default task
gulp.task('default', ['clean'], function (cb) {
    runSequence('styles', ['copyImages', 'templates', 'renderViews', 'watch', 'browser-sync'], cb);
});

// Build production files, the default task
gulp.task('deploy', ['clean'], function (cb) {
    runSequence('styles', ['bower', 'optimizeImages', 'templates', 'usemin'], cb);
});


gulp.task('sync', function (cb) {
    runSequence('styles', 'copyImages', 'templates', 'renderViews', 'templateCache', 'ts', 'concat', 'watch', 'browser-sync', cb);
});
