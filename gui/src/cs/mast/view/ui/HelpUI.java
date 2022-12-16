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
                "1.Perform the profiling using the appropriate button <br> " +
                "2.For more click on 'View details' and click again on the module whose logs you want to know"+
                "</html>");
        tutorialDescription.setFont(window.getFont().deriveFont(11f));
        centralPanel.add(tutorialDescription);

        JCheckBox check = new JCheckBox("Don't show the message again", true);
        lowPanel.add(check);

        JButton nextWindow = new JButton("Skip");
        nextWindow.addActionListener(e-> {
            if (!check.isSelected())
                firstLaunch.delete();
            window.showHome();
        });
        lowPanel.add(nextWindow);
    }
}