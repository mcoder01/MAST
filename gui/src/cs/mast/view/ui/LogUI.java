package cs.mast.view.ui;

import cs.mast.util.Log;
import cs.mast.util.LogParser;
import cs.mast.view.Window;

import javax.swing.*;
import java.util.ArrayList;

public class LogUI extends GenericUI {
    private final String moduleName;
    private JTextArea logArea;
    private int lastShownLog;


    public LogUI(Window window, String moduleName) {
        super(window);
        this.moduleName = moduleName;
        setLayout(null);
        lastShownLog = 0;
    }

    @Override
    public void addComponents() {

        addBackButton();
        addLogs();
    }

    @Override
    public void refresh() {
    	LogParser.getInstance().parse();
        ArrayList<Log> logs = LogParser.getInstance();
        if (logArea != null)
            for (; lastShownLog < logs.size(); lastShownLog++)
                showLog(logs.get(lastShownLog));
    }

    @Override
    protected void addBackButton() {
        super.addBackButton();
        backButton.addActionListener(e -> window.showDetails());
    }

    private void addLogs() {
        //Set name

        JLabel label = new JLabel("Logs from: " + moduleName);
        label.setBounds(0, 40, 600, 15);
        add(label);
        logArea = new JTextArea();
        logArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(logArea);
        scrollPane.setBounds(0, 60, 600, 513);
        scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        scrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        add(scrollPane);
    }

    private void showLog(Log log) {
        if (log.moduleName().equals(moduleName))
            logArea.append(log + "\n");
    }
}
