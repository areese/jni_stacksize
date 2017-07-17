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

    public static void main(String[] args) throws Exception {

        StackSize s = new StackSize();

        System.out.println("Main Stack size: " + (s.getStackSize() / MB) + " MB ");

        Thread t = new Thread(() -> {
            System.out.println("New thread Stack size: " + (s.getStackSize() / MB) + " MB ");
        });

        t.start();

        t.join();
    }
}
