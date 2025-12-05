import sqlite3
import os
import re
import datetime

# Configuration
DB_PATH = 'prompts.db' # Assumes script is run in same dir as db or user provides path
OUTPUT_DIR = 'prompts_tree' # Default output directory

def sanitize_filename(name):
    """Sanitize string to be safe for filenames."""
    # Remove invalid characters
    s = re.sub(r'[^a-zA-Z0-9_\-\s]', '', name)
    s = s.strip()
    if not s:
        s = "Untitled"
    return s

def format_date(timestamp_ms):
    """Convert unix timestamp (ms) to ISO format string."""
    # QDateTime::toMSecsSinceEpoch() stores milliseconds
    dt = datetime.datetime.fromtimestamp(timestamp_ms / 1000.0)
    return dt.isoformat()

def generate_frontmatter(title, created_at, updated_at):
    """Generate YAML frontmatter block."""
    fm = "---\n"
    fm += f"title: {title}\n"
    fm += f"createdAt: {created_at}\n"
    fm += f"updatedAt: {updated_at}\n"
    fm += "---\n"
    return fm

def migrate(db_path, output_path):
    if not os.path.exists(db_path):
        # unexpected, but let's look in standard paths
        standard_path = os.path.expanduser("~/.local/share/PromptManager/prompts.db")
        if os.path.exists(standard_path):
            print(f"Database not found at {db_path}, using {standard_path}")
            db_path = standard_path
        else:
            print(f"Error: Database file not found at {db_path} or default location.")
            return

    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    cursor = conn.cursor()

    # Load Folders
    folders = {} # id -> name
    try:
        cursor.execute("SELECT * FROM folders")
        for row in cursor.fetchall():
            folders[row['id']] = row['name']
    except sqlite3.OperationalError:
        print("No folders table found.")

    # Create directories
    os.makedirs(output_path, exist_ok=True)
    for folder_name in folders.values():
        clean_name = sanitize_filename(folder_name)
        folder_path = os.path.join(output_path, clean_name)
        os.makedirs(folder_path, exist_ok=True)
        print(f"Created folder: {clean_name}")

    # Load Prompts
    try:
        cursor.execute("SELECT * FROM prompts")
    except sqlite3.OperationalError:
        print("No prompts table found.")
        return

    count = 0
    for row in cursor.fetchall():
        title = row['title']
        content = row['content']
        folder_id = row['folder_id']
        created_at = format_date(row['created_at'])
        updated_at = format_date(row['updated_at'])

        # Determine path
        clean_title = sanitize_filename(title)
        filename = f"{clean_title}.md"
        
        if folder_id and folder_id in folders:
            folder_name = sanitize_filename(folders[folder_id])
            file_path = os.path.join(output_path, folder_name, filename)
        else:
            file_path = os.path.join(output_path, filename)

        # Write file
        full_content = generate_frontmatter(title, created_at, updated_at) + content
        
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(full_content)
        
        count += 1
        print(f"Exported: {file_path}")

    conn.close()
    print(f"\nMigration complete! exported {count} prompts to '{output_path}'.")
    print(f"You can now point the application to this directory.")

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='Migrate PromptManager SQLite DB to Markdown Tree')
    parser.add_argument('--db', default='prompts.db', help='Path to sqlite database file')
    parser.add_argument('--out', default=os.path.expanduser('~/prompts_tree'), help='Output directory')
    
    args = parser.parse_args()
    migrate(args.db, args.out)
