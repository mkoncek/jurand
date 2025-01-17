include rules.mk

.PHONY: force all clean test-compile test coverage manpages test-install clean-install
.DEFAULT_GOAL = all

CXXFLAGS += -g -std=c++2a -Isrc -Wall -Wextra -Wpedantic

JAVA_HOME ?= /usr/lib/jvm/java-21

all: $(call Executable_file,jurand)

test-compile: $(call Executable_file,jurand) $(call Executable_file,jurand_test)

test: test.sh test-compile
	@./$<

$(call Executable_file,jurand): $(call Object_file,jurand.cpp)
$(call Executable_file,jurand_test): $(call Object_file,jurand_test.cpp)

manpages: \
	$(call Manpage,jurand.1)\
	$(call Manpage,java_remove_annotations.7)\
	$(call Manpage,java_remove_imports.7)\

test-install: export buildroot = target/buildroot
test-install: export bindir = /usr/bin
test-install: export rpmmacrodir = /usr/lib/rpm/macros.d
test-install: export mandir = /usr/share/man
test-install: all manpages
	./install.sh

$(call Object_file,jurand_jni.cpp): CPPFLAGS += -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -Itarget/generated/jni
$(call Object_file,jurand_jni.cpp): CXXFLAGS += -fpic
$(call Object_file,jurand_jni.cpp): target/generated/jni/org_fedoraproject_javapackages_Jurand.h

target/lib/libjurand_jni.so: CXXFLAGS += -fpic -shared
target/lib/libjurand_jni.so: $(call Object_file,jurand_jni.cpp) | target/lib
	$(CXX) -o $@ -fpic -shared $(LDFLAGS) $(wordlist 2,$(words $^),$^) $(LDLIBS) $<

target/java/org/fedoraproject/javapackages/Jurand.class target/generated/jni/org_fedoraproject_javapackages_Jurand.h &: src/Jurand.java
	$(JAVA_HOME)/bin/javac -d target/java/ -h target/generated/jni $<

target/jurand-jni.jar: target/java/org/fedoraproject/javapackages/Jurand.class
	$(JAVA_HOME)/bin/jar -c -v -f $@ --main-class='org.fedoraproject.javapackages.Jurand' -C target/java $$(cd target/java && echo *)

target/javadoc: src/Jurand.java
	$(JAVA_HOME)/bin/javadoc -d $@ $<

clean-install:
	@rm -rfv target/buildroot

-include target/dependencies/*.mk
