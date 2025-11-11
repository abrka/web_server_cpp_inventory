# Note: the module name is psycopg, not psycopg3
import os
import psycopg2
import dotenv

dotenv.load_dotenv()

# Connect to an existing database
with psycopg2.connect(dsn=os.getenv("DATABASE_PUBLIC_URI")) as conn:  # 5432 as a default if not found
    # Open a cursor to perform database operations
    with conn.cursor() as cur:
        cur.execute("""
            CREATE TABLE IF NOT EXISTS categories( 
              id SERIAL PRIMARY KEY,
              name VARCHAR(255)
            );
        """)
        cur.execute("""
            CREATE TABLE IF NOT EXISTS categories( 
              id SERIAL PRIMARY KEY,
              name VARCHAR(255),
              category_id INT REFERENCES categories(id)
            );
        """)
        f = open('categories.csv')
        cur.copy_from(f, 'categories', columns=('id', 'name'), sep=",")
        f.close()
        f_2 = open('items.csv')
        cur.copy_from(f_2, 'items', columns=('id', 'name', 'category_id'), sep=",")
        f_2.close()
        # Make the changes to the database persistent
        conn.commit()