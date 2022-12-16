package cs.mast.util;

public record Log(String timestamp, String moduleName, Status status, String message) {
    @Override
    public String toString() {
        String s = "[" + timestamp + "] [" + status + "] " + moduleName;
        if (!message.isEmpty())
            s += ": " + message;
        return s;
    }

    public enum Status {
        RUNNING("Running...", "lime"),
        PROFILING("Profiling...", "#FFAA00"),
        ANOMALY("Anomaly!", "red");

        private final String text, color;
        Status(String text, String color) {
            this.text = text;
            this.color = color;
        }

        public String getText() {
            return text;
        }

        public String getColor() {
            return color;
        }
    }
}
