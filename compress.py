import sys
import os
import gzip
import base64

def pack_html(input_file):
    if not os.path.exists(input_file):
        print(f"❌ Error: File '{input_file}' not found.")
        sys.exit(1)

    output_file = input_file.replace(".html", ".packed.html")
    
    with open(input_file, 'r', encoding='utf-8') as f:
        html_content = f.read()

    print(f"--- Packing {input_file} ---")

    # 1. GZIP the raw HTML (No minification to prevent breaking Raylib glue code)
    compressed_data = gzip.compress(html_content.encode('utf-8'), compresslevel=9)
    
    # 2. Base64 encode
    b64_data = base64.b64encode(compressed_data).decode('utf-8')

    # 3. Escape to prevent HTML parser confusion
    safe_b64 = b64_data.replace("/", "\\/")

    # 4. The Bootloader
    # This writes the decoded content into a new document context.
    # We use a more robust Base64-to-Uint8Array loop.
    packed_template = f"""<!DOCTYPE html><html><head><meta charset="utf-8"><title>Loading...</title>
<style>body,html{{margin:0;padding:0;height:100%;width:100%;background:#000;color:#fff;display:flex;align-items:center;justify-content:center;font-family:sans-serif}}</style></head>
<body><div id="status">Loading Game...</div><script>
(async function(){{
    try {{
        const safeB64 = "{safe_b64}";
        const b64 = safeB64.replace(/\\\\\\//g, "/");
        const str = atob(b64);
        const len = str.length;
        const buf = new Uint8Array(len);
        for (let i = 0; i < len; i++) {{
            buf[i] = str.charCodeAt(i);
        }}
        
        const stream = new Response(buf).body.pipeThrough(new DecompressionStream("gzip"));
        const decoded = await new Response(stream).text();
        
        document.open();
        document.write(decoded);
        document.close();
    }} catch (e) {{
        console.error("Packer error:", e);
        document.getElementById("status").innerHTML = "<h1>Unpack Failed</h1><pre>" + e.message + "</pre>";
    }}
}})();
</script></body></html>"""

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(packed_template)

    orig_size = len(html_content)
    new_size = len(packed_template)
    reduction = 100 - (new_size / orig_size * 100)

    print(f"✅ Success!")
    print(f"Original: {orig_size:,} bytes")
    print(f"Packed:   {new_size:,} bytes")
    print(f"Saved:    {reduction:.2f}%")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python pack_html.py <filename.html>")
    else:
        pack_html(sys.argv[1])
