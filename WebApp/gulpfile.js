// -----------------------------------------------------------------------------
// File system builder
// -----------------------------------------------------------------------------

const gulp = require('gulp')
const async = require('async')
const del = require('del')
const glob = require('glob')
const path = require('path')
const fs = require('fs')
const cleanCss = require('gulp-clean-css')
const imagemin = require('gulp-imagemin')
const jsonminify = require('gulp-jsonminify')
const uglifyEs = require('gulp-uglify-es')
const plumber = require('gulp-plumber')
const htmlmin = require('gulp-htmlmin')
const gzip = require('gulp-gzip')
const gulpIf = require('gulp-if')

const sourcePath = 'www'
const destinationPath = 'data'
const filesToRemove = [
  destinationPath + '/**/.DS_Store',
  destinationPath + '/**/host.config.json',
  destinationPath + '/**/manifest.json',
  destinationPath + '/**/host.config.json.gz',
  destinationPath + '/**/manifest.json.gz',
]

/**
 * Task clean
 *
 * Clean destination folder
 */
gulp.task('clean', () => {
  return del(destinationPath)
})

/**
 * Task copy
 *
 * Copy files
 */
gulp.task('copy', () => {
  return gulp.src(sourcePath + '/**/*')
    .pipe(gulp.dest(destinationPath))
})

/**
 * Task minify
 *
 * Minify files
 */
gulp.task('minify', () => {
  return gulp.src(destinationPath + '/**/*')
    .pipe(plumber())
    .pipe(gulpIf('*.json', jsonminify()))
    .pipe(gulpIf('*.css', cleanCss()))
    .pipe(gulpIf('*.js', uglifyEs.default()))
    .pipe(gulpIf('*.html', htmlmin({
      collapseWhitespace: true,
      removecomments: true,
      aside: true,
      minifyCSS: true,
      minifyJS: true
    })))
    .pipe(gulp.dest(destinationPath))
})

/**
 * Task imagemin
 *
 * Minify images
 */
gulp.task('imagemin', () => {
  return gulp.src(destinationPath + '/**/*')
    .pipe(imagemin([
      imagemin.gifsicle({ interlaced: true }),
      imagemin.mozjpeg({ quality: 85, progressive: true }),
      imagemin.optipng({ optimizationLevel: 5 }),
      imagemin.svgo({
        plugins: [
          { removeViewBox: true },
          { cleanupIDs: true }
        ]
      })
    ], {
      verbose: false,
      silent: true
    }))
    .pipe(gulp.dest(destinationPath))
})

/**
 * Task gzip
 *
 * Compress files
 */
gulp.task('gzip', () => {
  return gulp.src([destinationPath + '/**/*.{css,js,svg,json}', '!' + destinationPath + '/**/*.gz'])
    .pipe(gzip())
    .pipe(gulp.dest(file => {
      del(file.history)
      return file.base
    }))
})

/**
 * Task cleanup
 *
 * Clean folder
 */
gulp.task('rm-unused-icons', done => {

  let iconNames = []

  fromDir = (startPath, filter, callback) => {

    if (!fs.existsSync(startPath)) {
      console.log('no dir ', startPath)
      return
    }

    let files = fs.readdirSync(startPath)

    for (let i = 0; i < files.length; i++) {

      let filename = path.join(startPath, files[i])
      let stat = fs.lstatSync(filename)

      if (stat.isDirectory()) {
        fromDir(filename, filter, callback)
      } else if (filter.test(filename)) {
        callback(filename, files[i])
      }
    }
  }

  justPrint = (filePath, fileName) => {
    console.log('-- found: ', fileName.split('.')[0])
  }

  addTolist = (filePath, fileName) => {
    iconNames.push({
      iconName: fileName.split('.')[0],
      filePath,
      used: false,
      source: ''
    })
  }

  checkIfUnsused = (filePath, fileName) => {
    let data = fs.readFileSync(filePath)
    iconNames.forEach((icon, index, iconList) => {
      if (data.includes(`${icon.iconName}`)) {
        iconList[index].used = true
      }
    })
  }

  //fromDir(destinationPath + '/b/svg', /\.svg$/, justPrint)
  fromDir(destinationPath + '/b/svg', /\.svg$/, addTolist)
  fromDir(destinationPath, /\.(js|css|html)$/, checkIfUnsused)

  iconNames.forEach(icon => {
    if (!icon.used) {
      fs.unlink(icon.filePath, (err) => {
        if (err) {
          console.error(err)
        }
      })
    }
  })

  done()
})

/**
 * Task buildfs
 *
 * Build file system
 */
gulp.task('buildfs', gulp.series(['clean', 'copy', 'minify', 'imagemin', 'rm-unused-icons', 'gzip'], done => {
  done()
}))

/**
 * Task cleanup
 *
 * Clean folder
 */
gulp.task('cleanup', done => {
  filesToRemove.map(fileToRemove => {
    glob(fileToRemove, (err, files) => {
      files.map(file => {
        del([file])
      })
    })
  })
  done()
})

/**
 * Task default
 */
gulp.task('default', gulp.series(['buildfs', 'cleanup'], done => {
  done()
}))
