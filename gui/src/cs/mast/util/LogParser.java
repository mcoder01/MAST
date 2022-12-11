package cs.mast.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;

public class LogParser extends ArrayList<Log> {
    private static final String logPath = "/tmp/mast_modules_status";

    private static LogParser instance;
    private File logFile;
    private BufferedReader reader;

    private LogParser() {
        super();
        parse();
    }

    public boolean parse(){
        if (reader == null)
            loadFile();

        if (logFile == null || logFile.length() == 0) {
            clear();
            return false;
        }

        try{
            while(reader.ready()){
                String line = reader.readLine();
                String[] lineInfo = line.split("\\|");
                String timestamp = lineInfo[0];
                String moduleName = lineInfo[1];
                Log.Status status = Log.Status.valueOf(lineInfo[2].toUpperCase());
                String message = "";
                if (lineInfo.length == 4)
                    message = lineInfo[3];
                add(new Log(timestamp, moduleName, status, message));
            }

            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    @Override
    public String toString(){
        String text = "";
        for(Log l : this)
            text += l.toString();

        return text;
    }

    public boolean foundAnyAnomaly() {
        for (Log l : this)
            if (l.status() == Log.Status.ANOMALY)
                return true;
        return false;
    }

    public Log.Status getModuleStatus(String moduleName) {
        for (int i = size()-1; i >= 0; i--) {
            Log log = get(i);
            if (log.moduleName().equals(moduleName)) {
                if (log.status() == Log.Status.RUNNING && i > 0)
                    continue;
                return log.status();
            }
        }

        return null;
    }

    public ArrayList<String> findModuleNames(){
        ArrayList<String> moduleNames = new ArrayList<>();
        for(Log l:this)
            if(!moduleNames.contains(l.moduleName()))
                moduleNames.add(l.moduleName());

        return moduleNames;
    }

    public void loadFile() {
        logFile = new File(logPath);
        if (logFile.exists()) {
            try {
                reader = new BufferedReader(new FileReader(logPath));
            } catch (FileNotFoundException e) {
                throw new RuntimeException(e);
            }
        }
    }

    public static LogParser getInstance() {
        if (instance == null)
            instance = new LogParser();
        return instance;
    }
}
