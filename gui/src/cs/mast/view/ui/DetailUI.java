package cs.mast.view.ui;

import cs.mast.util.Log;
import cs.mast.util.LogParser;
import cs.mast.view.Window;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;

public class DetailUI extends GenericUI {
    private JPanel mainPanel;
    private JButton[] buttons;

    public DetailUI(Window window){
        super(window);
        setLayout(null);
    }

    @Override
    public void addComponents() {
        mainPanel = new JPanel();
        mainPanel.setLayout(new GridLayout(2,2));
        mainPanel.setBounds(0, 30, 600, 540);
        add(mainPanel);
        addBackButton();
    }

    @Override
    public void refresh() {
        if (buttons == null)
            return;

        LogParser.getInstance().parse();
        ArrayList<String> moduleNames = LogParser.getInstance().findModuleNames();
        GridLayout layout = (GridLayout) mainPanel.getLayout();
        int n = layout.getRows()*layout.getColumns();
        for (int i = 0; i < n; i++)
            if(i<moduleNames.size()){
                Log.Status status = LogParser.getInstance().getModuleStatus(moduleNames.get(i));
                buttons[i].setText("<html><h2 style=\"text-align:center;\">" + moduleNames.get(i) +
                        "</h2><h3 style=\"text-align:center;color:" + status.getColor() + ";\">" + status.getText()
                        + "</h3></html>");
                buttons[i].setEnabled(true);
            }else{
                buttons[i].setText("Vuoto");
                buttons[i].setEnabled(false);
            }
    }

    @Override
    protected void addBackButton() {
        super.addBackButton();
        backButton.addActionListener(e -> window.showHome());
    }

    @Override
    public void invoke() {
        addComponents();
        for (Component c : mainPanel.getComponents())
            if (c instanceof JButton)
                mainPanel.remove(c);
        addModules();
        super.invoke();
    }

    private void addModules() {
        ArrayList<String> moduleNames = LogParser.getInstance().findModuleNames();
        GridLayout layout = (GridLayout) mainPanel.getLayout();
        int n = layout.getRows()*layout.getColumns();
        buttons = new JButton[n];
        for(int i=0;i<n;i++){
            buttons[i] = new JButton();
            buttons[i].setFont(window.getFont().deriveFont(16f));
            buttons[i].setToolTipText("Click here to view the logs");
            final int index = i;
            buttons[i].addActionListener(l -> window.showLogs(moduleNames.get(index)));
            mainPanel.add(buttons[i]);
        }
    }
}