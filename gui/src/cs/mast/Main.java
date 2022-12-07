package cs.mast;

import cs.mast.view.Window;
import cs.mast.view.ui.HelpUI;

import java.io.File;
import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        Window window = new Window();
        //File txtFile = new File("/var/MAST/.wanted");
        File txtFile = new File("/tmp/.wanted");
        if(txtFile.exists() && !txtFile.isDirectory()) {
            System.out.println("NOT first time");
           window.start();
        } else{
            System.out.println("first time");
            try {
                txtFile.createNewFile();
            } catch (IOException e) {}
            new HelpUI(window, txtFile).invoke();
        }
    }
}
