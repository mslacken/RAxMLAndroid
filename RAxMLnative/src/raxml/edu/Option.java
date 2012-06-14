package raxml.edu;

public class Option implements Comparable<Option> {
	private String name;
	private String data;
	private String path;
	public String getName() {
		return name;
	}
	public String getData() {
		return data;
	}
	public String getPath() {
		return path;
	}
	public Option(String _name,String _data, String _path) {
		name = _name;
		data = _data;
		path = _path;
	}
	@Override
	public int compareTo(Option _opt) {
		if(this.name != null)
			return
					this.name.toLowerCase().compareTo(_opt.getName().toLowerCase());
		else
			throw new IllegalArgumentException();
	}
}
