package cs.mast.view.ui;

import cs.mast.view.Window;

import javax.swing.*;
import java.awt.*;

public abstract class GenericUI extends JPanel {
    protected Window window;
    protected JButton backButton;

    protected GenericUI(Window window) {
        super();
        this.window = window;
    }

    public void invoke() {
        addComponents();
        window.setContentPane(this);
        window.pack();
    }

    protected void addBackButton() {
        backButton = new JButton("←");
        backButton.setBounds(5, 5, 50, 30);
        add(backButton);
    }

    protected JLabel getIconLabel() {
        return new JLabel(new ImageIcon(window.getAppIcon().getScaledInstance(128,128, Image.SCALE_SMOOTH)));
    }

    public abstract void addComponents();
    public abstract void refresh();
}
