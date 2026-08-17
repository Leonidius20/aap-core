
buildscript {
    val enable_asan: Boolean by extra(false)

    repositories {
        google()
        mavenCentral()
        maven ("https://plugins.gradle.org/m2/")
        maven ("https://maven.pkg.jetbrains.space/public/p/compose/dev")
    }
    dependencies {
        classpath (libs.tools.build.gradle)
        classpath (libs.kotlin.gradle.plugin)
        classpath (libs.dokka.gradle.plugin)
    }
}

plugins {
    id ("maven-publish")
    alias(libs.plugins.kotlinAndroid) apply false
    //id("io.github.gradle-nexus.publish-plugin") version "1.1.0"
}

apply { from ("${rootDir}/publish-root.gradle") }

subprojects {
    group = "org.androidaudioplugin"
    repositories {
        google()
        mavenLocal()
        mavenCentral()
        maven ("https://plugins.gradle.org/m2/")
        maven ("https://jitpack.io")
        maven ("https://maven.pkg.jetbrains.space/public/p/compose/dev")
    }
}

tasks.register<Delete>("clean") {
    delete(rootProject.buildDir)
}

// to diagnose prefab fails on jitpack
val printCxxLogs = tasks.register("printCxxLogs") {
    doLast {
        println("=== SEARCHING FOR CXX LOGS ===")
        // Look into every module's build and .cxx directories explicitly
        subprojects.forEach { subproject ->
            val cxxDir = file("${subproject.projectDir}/.cxx")
            val buildCxxDir = file("${subproject.buildDir}/intermediates/cxx")

            listOf(cxxDir, buildCxxDir).forEach { dir ->
                if (dir.exists()) {
                    dir.walkTopDown()
                        .filter { it.isFile && (it.name == "prefab_command" || it.name.endsWith(".json") || it.name.endsWith(".log")) }
                        .forEach { logFile ->
                            println("\n--- FILE: ${logFile.absolutePath} ---")
                            println(logFile.readText())
                        }
                }
            }
        }
    }
}
