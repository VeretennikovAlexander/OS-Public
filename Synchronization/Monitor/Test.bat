set JAVA_HOME=C:\Java\jdk1.7.0_79\

set PATH=%PATH%;%JAVA_HOME%\bin

del Test.class

javac Monitor.java

java -cp "%CD%" Test

pause