@echo off

set SDK_PATH=C:\Users\ass\AppData\Local\Android\Sdk
set ANDROID_JAR=%SDK_PATH%\platforms\android-36.1\android.jar
set D8_PATH=%SDK_PATH%\build-tools\37.0.0\d8.bat

echo [1/3] compiling java to .class
javac -cp "%ANDROID_JAR%" oxyconnect.java

echo [2/3] converting to .dex
call "%D8_PATH%" --output oxyconnect.jar oxyconnect.class

echo [3/3] cleaning up
del oxyconnect.class

echo made oxyconnect.jar
pause