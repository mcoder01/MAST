public class Data {
    private final int reads, writes, opens;

    public Data(int reads, int writes, int opens) {
        this.reads = reads;
        this.writes = writes;
        this.opens = opens;
    }

    public double[] toDoubleArray() {
        return new double[] {reads, writes, opens};
    }

    @Override
    public String toString() {
        return "[reads=" + reads + ", writes=" + writes + ", opens=" + opens + "]";
    }

    public int getReads() {
        return reads;
    }

    public int getWrites() {
        return writes;
    }

    public int getOpens() {
        return opens;
    }

    public static Data parseString(String line) {
        String[] info = line.split(" ");
        int reads = Integer.parseInt(info[0]);
        int writes = Integer.parseInt(info[2]);
        int opens = Integer.parseInt(info[4]);
        return new Data(reads, writes, opens);
    }
}
