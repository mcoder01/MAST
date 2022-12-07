package cs.mast.view.ui;

import cs.mast.view.Window;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;

public class HomeUI extends GenericUI {
    private JPanel highPanel, lowPanel;

    public HomeUI(Window window){
        super(window);
        setLayout(null);
    }

    @Override
    public void addComponents(){
        /*
        try{
            UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
        }catch(Exception e){
            e.printStackTrace();
        }*/
        addPanels();
        addWidgets();
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

        JLabel outputLabel = new JLabel(" ", SwingConstants.CENTER);
        outputLabel.setFont(window.getFont().deriveFont(22f));
        highPanel.add(outputLabel);

        JButton detailLabel = new JButton("Visualizza dettagli");
        detailLabel.setFont(window.getFont().deriveFont(10f));
        detailLabel.setBounds(200,50,200,20);
        detailLabel.setBorderPainted(false);
        detailLabel.setForeground(Color.BLUE);
        detailLabel.setFocusPainted(false);
        detailLabel.setToolTipText("Clicca qui per iniziare visualizzare tutti i dettagli");
        lowPanel.add(detailLabel);

        JButton startButton = new JButton("Inizia analisi");
        startButton.setFont(window.getFont().deriveFont(12f));
        startButton.setBounds(200,0,200,20);
        startButton.addActionListener(e -> {
            //String[] comands = {getClass().getResource("mast.service"};
            if(startButton.getText().equals("Inizia analisi")){
                ProcessBuilder pb = new ProcessBuilder("systemctl", "enable", "--now", "mast.service");
                //ProcessBuilder pb = new ProcessBuilder("echo","ciao");
                //pb.inheritIO();
                try {
                    Process p = pb.start();
                    outputLabel.setText("<html><h1 style='text-align:center;'>Mast service avviato<br>Analisi in corso</h1></html>");
                    int exitStatus = p.waitFor();
                    System.out.println(exitStatus);
                    startButton.setText("Stop analisi");
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
                    startButton.setText("Inizia analisi");
                    outputLabel.setText(" ");
                } catch (InterruptedException | IOException e1) {
                    e1.printStackTrace();
                }
            }
        });

        detailLabel.addActionListener(e -> window.showDetails());
        lowPanel.add(startButton);
    }
}
