import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    kotlin("jvm") version "1.3.50"
}

allprojects {
    apply(plugin = "kotlin")

    group = "com.guinness.bot"
    version = "1.0-SNAPSHOT"

    repositories {
        maven(url = "https://repo.spring.io/release")
        mavenCentral()
        maven(url = "https://jitpack.io")
    }

    tasks.withType<KotlinCompile>().configureEach {
        kotlinOptions {
            jvmTarget = "1.8"
            suppressWarnings = true
        }
    }
}