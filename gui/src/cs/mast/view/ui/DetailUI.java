package cs.mast.view.ui;

import cs.mast.util.LogParser;
import cs.mast.view.Window;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;

public class DetailUI extends GenericUI {
    private JPanel mainPanel;

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
    protected void addBackButton() {
        super.addBackButton();
        backButton.addActionListener(e -> window.showHome());
    }

    @Override
    public void invoke() {
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
        JButton[] buttons = new JButton[n];
        for(int i=0;i<n;i++){
            if(i<moduleNames.size()){
                LogParser.Status status = LogParser.getInstance().getModuleStatus(moduleNames.get(i));
                buttons[i] = new JButton("<html>" + moduleNames.get(i) + "<br> <p style=\"text-align:center;color:"
                        + status.getColor() + ";\">"  + status.getText() + "</p></html>");
            }else{
                buttons[i] = new JButton("Vuoto");
                buttons[i].setEnabled(false);
            }

            buttons[i].setFont(window.getFont().deriveFont(12f));
            buttons[i].setToolTipText("Clicca qui per visualizzare i log");
            final int index = i;
            buttons[i].addActionListener(l -> window.showLogs(moduleNames.get(index)));
            mainPanel.add(buttons[i]);
        }
    }

    /*
    public void addActions(){
        nextWindow.addActionListener(e-> {
            System.oust.println("Passa alla schermata principale");
            //w.setVisible(true);
            this.dispose();
            if (!check.isSelected()){
                f.delete();
            }
        });

    }
    */
}