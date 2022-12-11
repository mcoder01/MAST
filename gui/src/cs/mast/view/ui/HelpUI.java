package cs.mast.view.ui;

import cs.mast.view.Window;

import javax.swing.*;
import java.awt.*;
import java.io.File;

public class HelpUI extends GenericUI {
    private JPanel panel, centralPanel, lowPanel;
    private final File firstLaunch;

    public HelpUI(Window window, File firstLaunch){
        super(window);
        this.firstLaunch = firstLaunch;
        setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
    }

    @Override
    public void addComponents() {
        addPanels();
        addWidget();
    }

    @Override
    public void refresh() {}

    private void addPanels() {
        panel = new JPanel();
        panel.setPreferredSize(new Dimension(200,200));
        add(panel);

        centralPanel = new JPanel();
        add(centralPanel);

        lowPanel = new JPanel();
        lowPanel.setLayout(new BoxLayout(lowPanel,BoxLayout.X_AXIS));
        add(lowPanel);
    }

    public void addWidget(){
        panel.add(getIconLabel());

        JLabel tutorialDescription = new JLabel("<html>" +
                "1. Effettua il profiling cliccando sul play e cliccando su Start Profiling <br> " +
                "2. Clicca sull'opzione che vuoi utilizzare <br>" +
                "3. Per maggiori informazioni clicca su HELP [Commnad+A] <br>"+
                "</html>");
        tutorialDescription.setFont(window.getFont().deriveFont(11f));
        centralPanel.add(tutorialDescription);

        JCheckBox check = new JCheckBox("Non visualizzare piu il messagio", true);
        lowPanel.add(check);

        JButton nextWindow = new JButton("Salta");
        nextWindow.addActionListener(e-> {
            System.out.println("Passa alla schermata principale");
            if (!check.isSelected())
                firstLaunch.delete();
            window.showHome();
        });
        lowPanel.add(nextWindow);
    }
}