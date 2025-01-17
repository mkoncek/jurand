package org.fedoraproject.javapackages;

import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * Java annotation (and import) removal tool.
 */
public class Jurand {
	static {
		System.loadLibrary("jurand_jni");
	};
	
	private Jurand() {
	}
	
	/**
	 * Removes import statements on the set of fileroots.
	 * @param fileroots The files or directories on which the tool should be executed.
	 * @param names TODO
	 * @param patterns TODO
	 * @param strictMode TODO
	 * @throws IllegalArgumentException If the tool's fields or fileroots are incorrect.
	 * @throws RuntimeException If the tool encountered an error during its execution.
	 * May also happen in case of failures due to strict mode.
	 */
	public static void remove_imports(Path[] fileroots, String[] names, String[] patterns, boolean strictMode) {
		execute(fileroots, names, patterns, strictMode, false);
	}
	
	/**
	 * Removes the usage of annotations and import statements on the set of fileroots.
	 * @param fileroots The files or directories on which the tool should be executed.
	 * @param names TODO
	 * @param patterns TODO
	 * @param strictMode TODO
	 * @throws IllegalArgumentException If the tool's fields or fileroots are incorrect.
	 * @throws RuntimeException If the tool encountered an error during its execution.
	 * May also happen in case of failures due to strict mode.
	 */
	public static void remove_annotations(Path[] fileroots, String[] names, String[] patterns, boolean strictMode) {
		execute(fileroots, names, patterns, strictMode, true);
	}
	
	private static native void execute(Path[] fileroots, String[] names, String[] patterns, boolean strictMode, boolean remove_annotations);
}
