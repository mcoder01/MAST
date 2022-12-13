package cs.mast.view;

import cs.mast.view.ui.DetailUI;
import cs.mast.view.ui.GenericUI;
import cs.mast.view.ui.HomeUI;
import cs.mast.view.ui.LogUI;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;

public class Window extends JFrame implements Runnable {
    private GenericUI currentUi;
    private BufferedImage appIcon;
    private Font font;

    {
        try(InputStream iconStream = getClass().getClassLoader().getResourceAsStream("images/iconApp.png");
            InputStream fontStream = getClass().getClassLoader().getResourceAsStream("fonts/jack.ttf")) {
            if (iconStream != null)
                appIcon = ImageIO.read(iconStream);
            if (fontStream != null)
                font = Font.createFont(Font.TRUETYPE_FONT,fontStream);
        } catch (IOException | FontFormatException e) {
            e.printStackTrace();
        }
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
