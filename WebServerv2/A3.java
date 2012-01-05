import javax.swing.*;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import java.lang.reflect.Array;


public class A3 extends JFrame{

	//Set the Size Variables
	public static final int WIDTH = 550;
	public static final int HEIGHT = 300;

	//Set the serial number
	public static final long serialVersionUID = 1;

	//Pointer to the log area
	private JTextArea logArea;

	//Debug variable settings
	//0 = no debug messages
	//1 = standard debug messages
	//2 = verbose mode

	boolean serverUp;

	//Constructor
	public A3(){	

		super();
		
		//pointers for creating the Frame
		JMenuBar menuBar;
		JMenu menu;
		JMenuItem menuItem;
		JPanel panel;
		JScrollPane scroller;

		//Load native library
		System.loadLibrary("A3");

		//Size the Frame
		setSize(WIDTH,HEIGHT);
		
		//Set the title
		setTitle("Server GUI");
		
		//Define x button behavior
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		//Create the menu bar.
		menuBar = new JMenuBar();

		//Build the server Menu.
		menu = new JMenu("Server");
		menuItem = new JMenuItem("Pause");
		menuItem.addActionListener(new PauseListener());
		menu.add(menuItem);
		menuItem = new JMenuItem("Unpause");
		menuItem.addActionListener(new UnpauseListener());
		menu.add(menuItem);
		menuItem = new JMenuItem("Shutdown");
		menu.add(menuItem);
		menuItem.addActionListener(new ShutdownListener(this));
		menuBar.add(menu);
		
		//Build the Includes Menu.
		menu = MakeIncludeMenu("Includes");
		menuBar.add(menu);
		
		//Build the Logs Menu.
		menu = new JMenu("Logs");
		menuItem = new JMenuItem("View Logs");
		menuItem.addActionListener(new LogListener());
		menu.add(menuItem);
		menuBar.add(menu);
		
		//Add the menubar to the frame.
		this.setJMenuBar(menuBar);
		
		//Build the logs area.
		panel = new JPanel();
		logArea = new JTextArea(14,40);
		logArea.setEditable(false);
		scroller = new JScrollPane(logArea);
		scroller.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		panel.add(scroller);
		this.add(panel);

	}

	//Native Functions
	native int sendCommand(String command);
	native int openFifo(String name);
	native int openLFifo(String name);
	native String getlibs(String path);
	native String readlogs();
	native String readlogsfromfile();

	//Main function
	public static void main (String args[]){
		A3 me;
		
		//Load the Frame
		me = new A3();
		me.setVisible(true);
		
		//Connect to the pipe
		me.openFifo("Spipe");
		me.openLFifo("Lpipe");
		me.serverUp = true;
	}

	//Function to construct the Includes Menu
	private JMenu MakeIncludeMenu(String name){
		String[] names;
		JMenu ret;
		JMenuItem nmi;
		int i = 0;
		//get the names of the libraries
		String allNames = getlibs("./lib");

		//create the menu object
		ret = new JMenu(name);

		//tokenize the library name string
		names = allNames.split(" ");

		//add each library name to the menu
		for(i = 0; i < Array.getLength(names); i++){

			//add a new Menu Item
			nmi = new JMenuItem(names[i]);
			nmi.addActionListener(new IncludeListener(names[i]));
			ret.add(nmi);
		}
		
		//return the constructed menu
		return ret;
	}
	
	/**
	* Event Handler for the Includes menu Options
	*/
	private class IncludeListener implements ActionListener
	{
		private String name;

		//Constructor
		public IncludeListener(String label){
			super();
			this.name = label;
		}
		
		//Action handler
		public void actionPerformed(ActionEvent e)
		{
			//send the command through the pipe
			sendCommand("t " + this.name + ";");

			//log the action
			logArea.append("toggled Library '" + this.name + "'\n");
		}
	}

	/**
	* Event Handler for the Pause menu Option
	*/
	private class PauseListener implements ActionListener
	{
		public void actionPerformed(ActionEvent e)
		{
			//send the command through the pipe
			sendCommand("p");
			
			//log the action
			logArea.append("Paused server\n");
		}
	}

	/**
	* Event Handler for the Pause menu Option
	*/
	private class UnpauseListener implements ActionListener
	{
		public void actionPerformed(ActionEvent e)
		{

			//send the command through the pipe
			sendCommand("u");
			
			//log the action
			logArea.append("Unpaused server\n");
		}
	}

	/**
	* Event Handler for the Shutdown menu Option
	*/
	private class ShutdownListener implements ActionListener
	{
		A3 parent;

		ShutdownListener(A3 mommy){
			super();
			parent = mommy;
		}
		public void actionPerformed(ActionEvent e)
		{
			//send the command through the pipe
			sendCommand("s");

			parent.serverUp = false;

			//log the action
			logArea.append("Shutting down server\n");
		}
	}

	/**
	* Method to format a log string
	*/
	private String FormatLog(String log){
		String parts[];

		//tokenize the log string
		parts = log.split(" ");

		//if it is a log entry, return a formatted string ready to be displayed
		if (Array.getLength(parts) == 3){
			return ("Log Entry:\nPage: " + parts[0] + "\nBrowser: " + parts[1] +"\nDate: " + parts[2] + "\n\n");
		} else return "";
	}

	/**
	* Method to read a log file and generate a string to display all access logs*/
	private String ReadLogs(String path){
		String ret = "", logs[], allLogs;
		int i = 0;

		//read the log file
		if (this.serverUp) allLogs = readlogs();
		else allLogs = readlogsfromfile();

		if (!allLogs.equals("None") && !allLogs.equals("S;")){
			//tokenize the logs into individual log strings
			logs = allLogs.split(";");
					//format the log strings into a chain of displayable log blocks
			for(i = 0; i < Array.getLength(logs); i++){
				ret = ret + FormatLog(logs[i]);
			}
		} else if (allLogs.equals("S;")){
			logArea.append("Server down, Reading logs from file\n");
			this.serverUp = false;
		} else ret = "No logs to display\n";

		//return the displayable log block chain
		return ret;
	}

	/**
	* Event Handler for the Read Logs menu Option
	*/
	private class LogListener implements ActionListener
	{
		public void actionPerformed(ActionEvent e)
		{
			//load the logs and display them
			logArea.append(ReadLogs("access.log"));
		}
	}
}
