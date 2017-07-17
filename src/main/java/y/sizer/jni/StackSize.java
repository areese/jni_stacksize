package y.sizer.jni;

public class StackSize {
    static {
        StackSizeLibraryLoader.load();
    }

    public long getStackSize() {
        return StackSizeAccess.getStackSize();
    }

    public String getThreadInfo() {
        return StackSizeAccess.getThreadInfo();
    }


    private static final long MB = 1024 * 1024;

    private final void print(String prefix) {
        System.out.println(prefix + (getStackSize() / MB) + " MB \n" + getThreadInfo());
    }

    public static void main(String[] args) throws Exception {

        StackSize s = new StackSize();

        s.print("Main Stack size: ");

        Thread t = new Thread(() -> {
            s.print("New thread Stack size: ");
        });

        t.start();

        t.join();
    }
}
