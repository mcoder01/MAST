package cs.mast.view.ui;

import cs.mast.util.LogParser;
import cs.mast.view.Window;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;

public class HomeUI extends GenericUI {
    private JPanel highPanel, lowPanel;
    private JButton startButton, detailButton;
    private JLabel outputLabel;
    private boolean running;

    public HomeUI(Window window){
        super(window);
        setLayout(null);
    }

    @Override
    public void addComponents(){

        addPanels();
        addWidgets();
    }

    @Override
    public void refresh() {
        running = LogParser.getInstance().parse();
        String outputText, outputColor;
        if (running) {
            if (startButton != null)
                startButton.setText("Stop analysis");

            if (detailButton != null)
                detailButton.setEnabled(true);

            if (LogParser.getInstance().foundAnyAnomaly()) {
                outputText = "Some anomaly has been detected!";
                outputColor = "red";
            } else {
                outputText = "Service running";
                outputColor = "green";
            }
        } else {
            if (startButton != null)
                startButton.setText("Start analysis");

            if (detailButton != null)
                detailButton.setEnabled(false);

            outputText = "Service not running";
            outputColor = "gray";
        }

        if (outputLabel != null)
            outputLabel.setText("<html><p style=\"color: " + outputColor + ";\">" + outputText + "</p></html>");
    }

    protected void addPanels() {
        highPanel = new JPanel();
        highPanel.setLayout(new GridLayout(2, 1));
        highPanel.setBounds(0,0,600,400);
        add(highPanel);

        lowPanel = new JPanel();
        lowPanel.setLayout(null);
        lowPanel.setBounds(0,400,600,200);
        add(lowPanel);
    }

    protected void addWidgets(){
        highPanel.add(getIconLabel());

        outputLabel = new JLabel("",
                SwingConstants.CENTER);
        outputLabel.setFont(window.getFont().deriveFont(22f));
        highPanel.add(outputLabel);

        detailButton = new JButton("Show details");
        detailButton.setFont(window.getFont().deriveFont(10f));
        detailButton.setBounds(200,50,200,20);
        detailButton.setBorderPainted(false);
        detailButton.setForeground(Color.BLUE);
        detailButton.setFocusPainted(false);
        detailButton.setToolTipText("Click here to view all the details");
        detailButton.setEnabled(false);
        lowPanel.add(detailButton);

        startButton = new JButton();
        startButton.setFont(window.getFont().deriveFont(12f));
        startButton.setBounds(200,0,200,20);
        startButton.addActionListener(e -> {
            //String[] comands = {getClass().getResource("mast.service"};
            if(!running){
                ProcessBuilder pb = new ProcessBuilder("systemctl", "enable", "--now", "mast.service");
                //ProcessBuilder pb = new ProcessBuilder("echo","ciao");
                //pb.inheritIO();
                try {
                    Process p = pb.start();
                    int exitStatus = p.waitFor();
                    System.out.println(exitStatus);
                } catch (InterruptedException | IOException e1) {
                    e1.printStackTrace();
                }
            }else{
                ProcessBuilder pb = new ProcessBuilder("systemctl", "stop", "mast.service");
                //ProcessBuilder pb = new ProcessBuilder("echo","ciao");
                pb.inheritIO();
                try {
                    Process p = pb.start();
                    int exitStatus = p.waitFor();
                    System.out.println(exitStatus);
                } catch (InterruptedException | IOException e1) {
                    e1.printStackTrace();
                }
            }
        });

        detailButton.addActionListener(e -> window.showDetails());
        lowPanel.add(startButton);
    }
}
