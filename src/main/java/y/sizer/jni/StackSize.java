package y.sizer.jni;

public class StackSize {
    static {
        StackSizeLibraryLoader.load();
    }

    public long getStackSize() {
        return StackSizeAccess.getStackSize();
    }
}
