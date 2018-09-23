//-----------------------------------------------------------------------------
/** @file pentobi/qml/Controls.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

// Helper function to add a mnemonic without using an ampersand in the
// translatable MenuItem text or in the Action text.
// Ampersands in texts cause problems for translation with Weblate because
// Weblate does not support searching strings containing ampersands.
// Also, there seems to be a bug in Qt that in some cases registers mnemonics
// in Action texts used in a MenuItem as global shortcuts (last occurred with
// Qt 5.11.1)
function addMnemonic(text, mnemonic) {
    if (! isDesktop || mnemonic === "")
        return text
    mnemonic = mnemonic.toLowerCase()
    var textLower = text.toLowerCase()
    var pos = textLower.indexOf(mnemonic)
    // Prefer beginning of word
    while (pos >= 0 && pos < textLower.length) {
        if (pos === 0 || textLower.charAt(pos - 1) === " ")
            break
        pos = textLower.indexOf(mnemonic, pos + 1)
    }
    if (pos < 0 || pos >= textLower.length)
        pos = textLower.indexOf(mnemonic)
    if (pos < 0) {
        console.warn("mnemonic", mnemonic, "not found in", text)
        return text
    }
    return text.substring(0, pos) + "&" + text.substring(pos)
}
