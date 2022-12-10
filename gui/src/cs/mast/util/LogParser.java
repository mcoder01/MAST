package cs.mast.util;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;

public class LogParser extends ArrayList<Log> {
    private static final String logPath = "/tmp/mast_modules_status";
    //private static final String logPath = "res/prova.txt";

    private static LogParser instance;
    private BufferedReader reader;

    private LogParser() {
        super();
        setup();
    }

    private void setup() {
        try {
            reader = new BufferedReader(new FileReader(logPath));
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        }
        parse();
    }

    public void parse(){
        try{
            while(reader.ready()){
                String line = reader.readLine();
                String[] lineInfo = line.split("\\|");
                String timestamp = lineInfo[0];
                String moduleName = lineInfo[1];
                String status = lineInfo[2];
                String message = "";
                if (lineInfo.length == 4)
                    message = lineInfo[3];
                Log log = new Log(timestamp, moduleName, status, message);
                add(log);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public ArrayList<Log> getLogsByName(String moduleName){
        ArrayList<Log> list = new ArrayList<>();
        for(Log l:this)
            if(l.moduleName().equals(moduleName))
                list.add(l);

        return list;
    }

    @Override
    public String toString(){
        String text = "";
        for(Log l : this)
            text += l.toString();

        return text;
    }

    public boolean findAnomalies(String moduleName){
        for(Log l:this){
            if(l.status().equals("Anomaly") && l.moduleName().equals(moduleName)){
                return true;
            }
        }
        return false;
    }

    public ArrayList<String> findModuleNames(){
        ArrayList<String> moduleNames = new ArrayList<>();
        for(Log l:this)
            if(!moduleNames.contains(l.moduleName()))
                moduleNames.add(l.moduleName());

        return moduleNames;
    }

    public static LogParser getInstance() {
        if (instance == null)
            instance = new LogParser();
        return instance;
    }
}
