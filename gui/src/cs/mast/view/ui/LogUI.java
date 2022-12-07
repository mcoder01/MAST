package cs.mast.view.ui;

import cs.mast.util.Log;
import cs.mast.util.LogParser;
import cs.mast.view.Window;

import javax.swing.*;

public class LogUI extends GenericUI implements Runnable {
    private final String moduleName;
    private JTextArea logArea;

    public LogUI(Window window, String moduleName) {
        super(window);
        this.moduleName = moduleName;
        setLayout(null);

        Thread thread = new Thread(this);
        thread.start();
    }

    @Override
    public void addComponents() {
        addBackButton();
        addLogs();
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
        logArea.setBounds(0, 60, 600, 550);
        logArea.setEditable(false);
        add(logArea);
    }

    @Override
    public void run() {
        int lastShownLog = 0;
        while(true) {
            LogParser.getInstance().parse();
            if (logArea != null)
                for (; lastShownLog < LogParser.getInstance().size(); lastShownLog++)
                    showLog(LogParser.getInstance().get(lastShownLog));
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {}
        }
    }

    private void showLog(Log log) {
        if (log.moduleName().equals(moduleName))
            logArea.append(log + "\n");
    }
}
