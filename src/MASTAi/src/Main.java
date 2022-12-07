import manuel.ai.neuralnetwork.MatrixNeuralNetwork;

import java.io.*;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        MatrixNeuralNetwork nn = new MatrixNeuralNetwork(3, 4, 2);
        File dataFile = new File("output");
        if (!dataFile.exists()) {
            System.out.println("Il file " + dataFile.getAbsolutePath() + " non esiste!");
            return;
        }

        try(Scanner scanner = new Scanner(new FileInputStream(dataFile))) {
            while(scanner.hasNextLine()) {
                Data newData = Data.parseString(scanner.nextLine());
                for (int i = 0; i < 100; i++)
                    nn.train(newData.toDoubleArray(), new double[] {1, 0});
            }
        } catch(IOException e) {
            System.err.println("Impossibile aprire il file output!");
        }

        File analyzed = new File("analyzed");
        if (!analyzed.exists()) return;

        try(Scanner scanner = new Scanner(new FileInputStream(analyzed))) {
            while(scanner.hasNextLine()) {
                Data readData = Data.parseString(scanner.nextLine());
                double[] output = nn.predict(readData.toDoubleArray());
                if (output[0] > output[1])
                    System.out.println(readData + " Nessuna anomalia!");
                else System.out.println(readData + " Anomalia!");
            }
        } catch(IOException e) {
            System.out.println("Impossibile leggere il file analyzed!");
        }
    }
}