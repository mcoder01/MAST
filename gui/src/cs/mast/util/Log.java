package cs.mast.util;

public record Log(String timestamp, String moduleName, String status, String message) {
    @Override
    public String toString() {
        String s = "[" + timestamp + "] [" + status + "] " + moduleName;
        if (!message.isEmpty())
            s += ": " + message;
        return s;
    }
}
