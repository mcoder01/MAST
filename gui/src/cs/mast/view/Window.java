package cs.mast.view;

import cs.mast.view.ui.DetailUI;
import cs.mast.view.ui.GenericUI;
import cs.mast.view.ui.HomeUI;
import cs.mast.view.ui.LogUI;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class Window extends JFrame {
    private GenericUI home, details;
    private BufferedImage appIcon;
    private Font font;

    {
        try {
            appIcon = ImageIO.read(new File("res/iconApp.png"));
            font = Font.createFont(Font.TRUETYPE_FONT,new File("res/font/jack.ttf"));
        } catch (IOException | FontFormatException e) {}
    }

    public Window() {
        setBackground(new Color(255, 255, 255));
        //Set Frame
        setPreferredSize(new Dimension(600,600));
        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        setResizable(false);
        pack();
        setLocationRelativeTo(null);
        setFont(font);
        setVisible(true);
    }

    public void start() {
        addMenu();
        showHome();
    }

    private void addMenu() {
        JMenuBar mb = new JMenuBar();
        JMenu x = new JMenu("MAST");
        x.setFont(font.deriveFont(14f));

        JMenuItem m1 = new JMenuItem("Help");
        m1.setAccelerator(KeyStroke.getKeyStroke('A', Toolkit.getDefaultToolkit().getMenuShortcutKeyMaskEx()));
        m1.setToolTipText("Premi qui per visualizzare le istruzioni del programma");
        m1.setFont(font.deriveFont(12f));
        x.add(m1);
        setJMenuBar(mb);
    }

    public void showHome() {
        if (home == null) {
            home = new HomeUI(this);
            home.addComponents();
        }

        home.invoke();
    }

    public void showDetails() {
        if (details == null) {
            details = new DetailUI(this);
            details.addComponents();
        }

        details.invoke();
    }

    public void showLogs(String moduleName) {
        LogUI logs = new LogUI(this, moduleName);
        logs.addComponents();
        logs.invoke();
    }

    public BufferedImage getAppIcon() {
        return appIcon;
    }
}
