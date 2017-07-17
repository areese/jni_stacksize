/* Copyright 2017 Yahoo Inc. */
/* Licensed under the terms of the 3-Clause BSD license. See LICENSE file in the project root for details. */
package y.sizer.jni;


public class StackSizeLibraryLoader {
    static final String LIBRARY = "stacksize";
    static {
        try {
            System.err.println("Loading " + LIBRARY + " native library");
            System.loadLibrary(LIBRARY);
            System.err.println("Loaded " + LIBRARY + " native library");
        } catch (UnsatisfiedLinkError error) {
            String javaLibraryPath = System.getProperty("java.library.path");

            System.err.println("Cannot load " + LIBRARY + " native library, -Djava.library.path="
                            + ((null == javaLibraryPath) ? "unset" : javaLibraryPath));
            error.printStackTrace();
            System.err.println("**********************************************************************************************************************\n"
                            + "**********************************************************************************************************************\n"
                            + "***                                                                                                                ***\n"
                            + "***                                                                                                                ***\n"
                            + "***    Cannot load "
                            + LIBRARY
                            + " native library appears to already have been loaded.                                 ***\n"
                            + "***                                                                                                                ***\n"
                            + "***                                                                                                                ***\n"
                            + "**********************************************************************************************************************\n"
                            + "**********************************************************************************************************************\n");
            throw error;
        }
    }

    public static void load() {
        // yes it's empty;
    }
}
