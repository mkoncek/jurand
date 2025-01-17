package org.fedoraproject.javapackages;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.lang.foreign.Arena;
import java.lang.foreign.FunctionDescriptor;
import java.lang.foreign.Linker;
import java.lang.foreign.MemorySegment;
import java.lang.foreign.SymbolLookup;
import java.lang.foreign.ValueLayout;
import java.lang.invoke.MethodHandle;

/**
 * Java annotation (and import) removal tool.
 */
public class Jurand {
	static {
		System.loadLibrary("jurand_ffm");
	};
	
	private static final Linker linker = Linker.nativeLinker();
	private static final SymbolLookup lookup = SymbolLookup.loaderLookup();
	
	private Jurand() {
	}
	
	public static enum Flag {
	    also_annotations,
        in_place,
        strict,
	}
	
	public static class V0 {
	    private static final MethodHandle jurand_execute_v0 = linker.downcallHandle(
	            lookup.find("jurand_execute_v0").get(),
	            FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.ADDRESS,
	                    ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_LONG));
	    
	    /**
	     * Removes import statements on the set of fileroots.
	     * @param fileroots The files or directories on which the tool should be executed.
	     * @param names TODO
	     * @param patterns TODO
	     * @param flags TODO
	     * @throws IllegalArgumentException If the tool's fields or fileroots are incorrect.
	     * @throws RuntimeException If the tool encountered an error during its execution.
	     * May also happen in case of failures due to strict mode.
	     */
	    public static void execute(Path[] fileroots, String[] names, String[] patterns, Flag... flags) throws RuntimeException {
	        long encoded_flags = 0;
	        for (var flag : flags) {
	            encoded_flags |= 1 << flag.ordinal();
	        }
	        
	        var argFileroots = new ByteArrayOutputStream();
	        var argNames = new ByteArrayOutputStream();
	        var argPatterns = new ByteArrayOutputStream();
	        
	        try {
	            for (var fileroot : fileroots) {
	                argFileroots.write(fileroot.toString().getBytes(StandardCharsets.UTF_8));
	                argFileroots.write(0);
	            }
	            argFileroots.write(0);
	            
	            for (var name : names) {
	                argNames.write(name.toString().getBytes(StandardCharsets.UTF_8));
	                argNames.write(0);
	            }
	            argNames.write(0);
	            
	            for (var pattern : patterns) {
	                argPatterns.write(pattern.toString().getBytes(StandardCharsets.UTF_8));
	                argPatterns.write(0);
	            }
	            argPatterns.write(0);
	        } catch (IOException ex) {
	            throw new RuntimeException(ex);
	        }
	        
	        long result;
	        try (var arena = Arena.ofShared()) {
	            var output = arena.allocate(256 * Runtime.getRuntime().availableProcessors());
	            result = (int) jurand_execute_v0.invokeExact(output,
	                    arena.allocateArray(ValueLayout.JAVA_BYTE, argFileroots.toByteArray()),
	                    arena.allocateArray(ValueLayout.JAVA_BYTE, argNames.toByteArray()),
	                    arena.allocateArray(ValueLayout.JAVA_BYTE, argPatterns.toByteArray()),
	                    encoded_flags);
	            if (result != 0) {
	                System.out.println(output.getUtf8String(0));
	            }
	        } catch (Throwable thr) {
	            throw new RuntimeException(thr);
	        }
	    }
	}
}
