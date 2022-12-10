package cs.mast;

import cs.mast.view.Window;
import cs.mast.view.ui.HelpUI;

import java.io.File;
import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        Window window = new Window();
        String homeDir = System.getProperty("user.home");
        File txtFile = new File(homeDir + "/.wanted");
        if(txtFile.exists() && !txtFile.isDirectory()) {
            System.out.println("NOT first time");
            window.showHome();
        } else{
            System.out.println("first time");
            try {
                txtFile.createNewFile();
            } catch (IOException e) {}

            HelpUI helpUi = new HelpUI(window, txtFile);
            helpUi.addComponents();
            helpUi.invoke();
        }
    }
}
