using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.InputSystem;
using TMPro;

[RequireComponent(typeof(Rigidbody2D))]
public class Player : MonoBehaviour
{
    [Header("Movement")]
    [SerializeField] private float moveSpeed = 8f;
    [SerializeField] private float acceleration = 50f;
    [SerializeField] private float deceleration = 50f;

    [Header("Jumping")]
    [SerializeField] private float jumpForce = 12f;
    [SerializeField] private float fallMultiplier = 2.5f;
    [SerializeField] private float lowJumpMultiplier = 2f;

    [Header("Ground Check")]
    [SerializeField] private Transform groundCheck;
    [SerializeField] private float groundCheckRadius = 0.2f;
    [SerializeField] private LayerMask groundLayer;

    [Header("Respawn & UI")]
    [SerializeField] private Transform spawnPoint;
    [SerializeField] private GameObject victoryPanel;
    [SerializeField] private GameObject pausePanel;
    [SerializeField] private TextMeshProUGUI timerText;
    [SerializeField] private TextMeshProUGUI finalTimeText;

    private Rigidbody2D rb;
    private float horizontalInput;
    private bool isGrounded;
    private bool jumpRequested;
    private bool jumpHeld;
    private Vector3 startPosition;

    // State & Timer variables
    private float elapsedTime = 0f;
    private bool isTimerRunning = true;
    private bool isPaused = false;

    // Track player stats for UI and database telemetry
    public int deathCount = 0;
    public int jumpCount = 0;

    private void Awake()
    {
        // Reset time scale in case scene was reloaded while paused
        Time.timeScale = 1f;

        // Grab the Rigidbody attached to the player
        rb = GetComponent<Rigidbody2D>();

        // Remember where the player started in case spawnPoint isn't set
        startPosition = transform.position;

        // Ensure UI panels are hidden when starting the level
        if (victoryPanel != null) victoryPanel.SetActive(false);
        if (pausePanel != null) pausePanel.SetActive(false);
    }

    private void Update()
    {
        // Toggle pause menu with Escape key
        if (Keyboard.current != null && Keyboard.current.escapeKey.wasPressedThisFrame)
        {
            TogglePause();
        }

        // Do not update movement or timer logic while paused
        if (isPaused) return;

        // Update timer every frame while active
        if (isTimerRunning)
        {
            elapsedTime += Time.deltaTime;
            UpdateTimerDisplay(timerText, elapsedTime);
        }

        // Read horizontal movement input (A/D or Left/Right arrow keys)
        horizontalInput = 0f;
        if (Keyboard.current != null)
        {
            if (Keyboard.current.aKey.isPressed || Keyboard.current.leftArrowKey.isPressed)
                horizontalInput -= 1f;
            if (Keyboard.current.dKey.isPressed || Keyboard.current.rightArrowKey.isPressed)
                horizontalInput += 1f;

            // Trigger a jump request when spacebar is pressed while grounded
            if (Keyboard.current.spaceKey.wasPressedThisFrame && isGrounded)
            {
                jumpRequested = true;
            }

            // Track if spacebar is being held to favour higher jumps
            jumpHeld = Keyboard.current.spaceKey.isPressed;
        }

        // Check if the detection circle at the feet touches the ground layer
        if (groundCheck != null)
        {
            isGrounded = Physics2D.OverlapCircle(groundCheck.position, groundCheckRadius, groundLayer);
        }
    }

    private void FixedUpdate()
    {
        // Physics updates must be calculated here
        Move();
        Jump();
        ApplyMarioPhysics();
    }

    private void Move()
    {
        float targetSpeed = horizontalInput * moveSpeed;
        float accelRate = (Mathf.Abs(targetSpeed) > 0.01f) ? acceleration : deceleration;

        Vector2 currentVel = rb.linearVelocity;
        float newVelocityX = Mathf.MoveTowards(currentVel.x, targetSpeed, accelRate * Time.fixedDeltaTime);
        rb.linearVelocity = new Vector2(newVelocityX, currentVel.y);

        if (horizontalInput > 0)
        {
            transform.localScale = Vector3.one;
        }
        else if (horizontalInput < 0)
        {
            transform.localScale = new Vector3(-1, 1, 1);
        }
    }

    private void Jump()
    {
        if (jumpRequested)
        {
            rb.linearVelocity = new Vector2(rb.linearVelocity.x, jumpForce);
            jumpRequested = false;
            jumpCount++;
        }
    }

    private void ApplyMarioPhysics()
    {
        Vector2 vel = rb.linearVelocity;

        if (vel.y < 0)
        {
            vel += Vector2.up * (Physics2D.gravity.y * (fallMultiplier - 1) * Time.fixedDeltaTime);
        }
        else if (vel.y > 0 && !jumpHeld)
        {
            vel += Vector2.up * (Physics2D.gravity.y * (lowJumpMultiplier - 1) * Time.fixedDeltaTime);
        }

        rb.linearVelocity = vel;
    }

    public void TogglePause()
    {
        isPaused = !isPaused;

        if (pausePanel != null)
        {
            pausePanel.SetActive(isPaused);
        }

        // Freeze physics and time when paused
        Time.timeScale = isPaused ? 0f : 1f;
    }

    public void Respawn()
    {
        deathCount++;

        // Reset velocity so momentum doesn't carry over after dying
        rb.linearVelocity = Vector2.zero;

        if (spawnPoint != null)
        {
            transform.position = spawnPoint.position;
        }
        else
        {
            transform.position = startPosition;
        }
    }

    private void OnTriggerEnter2D(Collider2D other)
    {
        if (other.CompareTag("DeathBox"))
        {
            Respawn();
        }
        else if (other.CompareTag("Finish"))
        {
            isTimerRunning = false;

            // Activate panel first
            if (victoryPanel != null)
            {
                victoryPanel.SetActive(true);
            }

            // Update final time text second
            if (finalTimeText != null)
            {
                UpdateTimerDisplay(finalTimeText, elapsedTime);
            }

            // Pass this Player instance to TelemetryManager to read variables
            TelemetryManager telemetry = FindFirstObjectByType<TelemetryManager>();
            if (telemetry != null)
            {
                telemetry.SendData(this);
            }
        }
    }

    public float GetElapsedTime()
    {
        return elapsedTime;
    }

    private void UpdateTimerDisplay(TextMeshProUGUI targetText, float timeToDisplay)
    {
        if (targetText == null) return;

        int minutes = Mathf.FloorToInt(timeToDisplay / 60);
        int seconds = Mathf.FloorToInt(timeToDisplay % 60);
        int milliseconds = Mathf.FloorToInt((timeToDisplay * 100) % 100);

        targetText.text = string.Format("{0:00}:{1:00}.{2:00}", minutes, seconds, milliseconds);
    }

    private void OnDrawGizmosSelected()
    {
        if (groundCheck != null)
        {
            Gizmos.color = Color.red;
            Gizmos.DrawWireSphere(groundCheck.position, groundCheckRadius);
        }
    }
}