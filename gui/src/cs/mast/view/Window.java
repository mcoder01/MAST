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

public class Window extends JFrame implements Runnable {
    private GenericUI currentUi;
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

        Thread thread = new Thread(this);
        thread.start();
    }

    public void showHome() {
        showUI(new HomeUI(this));
    }

    public void showDetails() {
        showUI(new DetailUI(this));
    }

    public void showLogs(String moduleName) {
        showUI(new LogUI(this, moduleName));
    }

    private void showUI(GenericUI ui) {
        currentUi = ui;
        currentUi.invoke();
    }

    public BufferedImage getAppIcon() {
        return appIcon;
    }

    @Override
    public void run() {
        while(true) {
            if (currentUi != null)
                currentUi.refresh();

            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }
}
