package lib;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class CHAT {
	
	private static CHAT defaultInstance;

	private BufferedReader reader;
	
	private CHAT() {
		reader = new BufferedReader( new InputStreamReader( System.in ) );
	}
	
	private static BufferedReader getReader() {
		if ( null == defaultInstance ) defaultInstance = new CHAT();
		return defaultInstance.reader;
	}
	
	public static int checkread( BPInteger error ) {
		try {
			if ( System.in.available() > 0 ) {
				return 0;
			}
			else {
				error.setValue( 35 );
				return -1;
			}
		} catch (IOException e) {
			error.setValue( 99 );
			return -1;
		}
	}
	
	public static int input( BPString input, BPInteger error ) {
		BufferedReader reader = getReader();
		try {
			input.setValue( reader.readLine() );
		    return 0;
		} catch (IOException e) {
			input.setValue( "" );
			error.setValue( 98 );
			return -1;
		}
	}

}
